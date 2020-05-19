#include "DatasetInner.h"

#include <Qt5Quazip/quazipfile.h>
#include <QApplication>
#include <QDir>
#include <QDomDocument>
#include <QTextStream>

#include "Common/Constants.h"
#include "Shared/Logger.h"

const char* DatasetInner::datasetsDir_ = "Data";

DatasetInner::DatasetInner(const QString& name, QObject* parent)
    : Dataset(name, parent)
{
    zip_.setZipName(DatasetInner::getDatasetsDir() + name +
                    Constants::getDatasetExtension());
    valid_ = load();
}

QStringList DatasetInner::getListOfAvailableDatasets()
{
    QDir datasetsDir(getDatasetsDir());

    if (!datasetDirExistAndUserHavePermisions())
    {
        return QStringList();
    }

    datasetsDir.setFilter(QDir::Files | QDir::Readable | QDir::NoSymLinks |
                          QDir::NoDotAndDotDot);
    datasetsDir.setNameFilters(
        QStringList("*" + Constants::getDatasetExtension()));
    datasetsDir.setSorting(QDir::Name);

    QStringList entries = datasetsDir.entryList();
    return entries.replaceInStrings(Constants::getDatasetExtension(),
                                    QLatin1String(""));
}

QString DatasetInner::getDatasetsDir()
{
    return QString(QApplication::applicationDirPath() + "/" + datasetsDir_ +
                   "/");
}

bool DatasetInner::analyze() { return true; }

bool DatasetInner::datasetDirExistAndUserHavePermisions()
{
    QDir directory(getDatasetsDir());

    if (!directory.exists())
    {
        bool created = directory.mkpath(directory.path());

        if (!created)
        {
            return false;
        }
    }

    return QFile::permissions(directory.path()).testFlag(QFile::ReadUser) &&
           QFile::permissions(directory.path()).testFlag(QFile::WriteUser);
}

bool DatasetInner::removeDataset(const QString& name)
{
    QString datasetFile =
        getDatasetsDir() + name + Constants::getDatasetExtension();
    return QFile::remove(datasetFile);
}

bool DatasetInner::loadData()
{
    // If definition is not valid than skip loading data.
    if (!isValid())
        return false;

    data_.resize(rowCount());

    const int activeColumnsCount = activeColumns_.size();
    for (int i = 0; i < rowCount(); ++i)
    {
        data_[i].resize(activeColumnsCount);
    }

    // Open archive.
    if (!zip_.open(QuaZip::mdUnzip))
    {
        LOG(LogTypes::IMPORT_EXPORT, "Can not open file " + zip_.getZipName());
        return false;
    }

    bool result = fillData(zip_, data_, false);
    zip_.close();
    if (result)
    {
        valid_ = true;
        sharedStrings_ = getSharedStringTable();
    }

    return result;
}

std::unique_ptr<QVariant[]> DatasetInner::getSharedStringTable()
{
    return std::move(stringsTable_);
}

void DatasetInner::updateSampleDataStrings()
{
    for (int i = 0; i < columnsCount_; ++i)
    {
        if (ColumnType::STRING == columnTypes_.at(i))
        {
            for (auto& sampleDataRow : sampleData_)
            {
                sampleDataRow[i] =
                    stringsTable_[sampleDataRow[i].toULongLong()];
            }
        }
    }
}

bool DatasetInner::load()
{
    // Open archive.
    bool result = zip_.open(QuaZip::mdUnzip);
    if (!result)
    {
        return false;
    }

    QByteArray definitionContent;

    result = loadXmlFile(definitionContent, zip_);

    if (!result)
    {
        zip_.close();
        return false;
    }

    result = fromXml(definitionContent);

    if (!result)
    {
        zip_.close();
        return false;
    }

    result = loadStrings(zip_);
    if (!result)
    {
        zip_.close();
        return false;
    }

    int rowsCountForSamples =
        (rowCount() > SAMPLE_SIZE ? SAMPLE_SIZE : rowCount());
    sampleData_.resize(rowsCountForSamples);

    for (int i = 0; i < rowsCountForSamples; ++i)
    {
        sampleData_[i].resize(columnsCount_);
    }

    result = fillData(zip_, sampleData_, true);

    if (!result)
    {
        zip_.close();
        return false;
    }

    // Set proper strings for sample data.
    updateSampleDataStrings();

    zip_.close();

    return result;
}

bool DatasetInner::fromXml(QByteArray& definitionContent)
{
    headerColumnNames_.clear();
    columnTypes_.clear();
    specialColumns_.clear();

    QDomDocument xmlDocument;

    // If parsing failure than exit.
    if (!xmlDocument.setContent(definitionContent))
    {
        LOG(LogTypes::IMPORT_EXPORT, "Xml file is corrupted.");
        return false;
    }

    LOG(LogTypes::IMPORT_EXPORT, "Read xml file:\n" + xmlDocument.toString());

    QDomElement root = xmlDocument.documentElement();

    // Load columns elements.
    QDomNodeList columns = root.firstChildElement(DATASET_COLUMNS).childNodes();

    LOG(LogTypes::IMPORT_EXPORT,
        "Read column count: " + QString::number(columns.count()));

    columnsCount_ = columns.size();
    for (int i = 0; i < columnsCount_; ++i)
    {
        QDomElement column = columns.at(i).toElement();
        headerColumnNames_.push_back(column.attribute(DATASET_COLUMN_NAME));
        columnTypes_.push_back(static_cast<ColumnType>(
            column.attribute(DATASET_COLUMN_FORMAT).toInt()));

        QString special = column.attribute(DATASET_COLUMN_SPECIAL_TAG);
        if (0 != special.compare(QLatin1String("")))
        {
            specialColumns_[static_cast<SpecialColumn>(special.toInt())] = i;
        }
    }

    // Read row count.
    const QString rowCountTag(DATASET_ROW_COUNT);
    rowsCount_ =
        root.firstChildElement(rowCountTag).attribute(rowCountTag).toInt();

    return true;
}

bool DatasetInner::loadXmlFile(QByteArray& definitionContent, QuaZip& zip)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(Constants::getDatasetDefinitionFilename());

    bool result = zipFile.open(QIODevice::ReadOnly);
    if (!result)
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open xml file " +
                Constants::getDatasetDefinitionFilename() + ".");
        return result;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Xml file " + Constants::getDatasetDefinitionFilename() + " opened.");

    definitionContent = zipFile.readAll();
    zipFile.close();

    return true;
}

bool DatasetInner::loadStrings(QuaZip& zip)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(Constants::getDatasetStringsFilename());

    bool result = zipFile.open(QIODevice::ReadOnly);
    if (!result)
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open strings file " +
                Constants::getDatasetStringsFilename() + ".");
        return result;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Strings file " + Constants::getDatasetStringsFilename() + " opened.");

    QByteArray stringsContent = zipFile.readAll();
    zipFile.close();

    QList<QByteArray> strings = stringsContent.split('\n');
    // First element is empty.
    stringsTable_ =
        std::make_unique<QVariant[]>(static_cast<size_t>(strings.size()) + 1);
    stringsTable_[0] = QVariant(QString());
    size_t counter = 1;
    for (const auto& currentString : strings)
    {
        stringsTable_[counter] = QVariant(QString::fromUtf8(currentString));
        counter++;
    }

    return true;
}

bool DatasetInner::fillData(QuaZip& zip,
                            QVector<QVector<QVariant> >& dataContainer,
                            bool fillSamplesOnly)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(Constants::getDatasetDataFilename());

    bool result = zipFile.open(QIODevice::ReadOnly);
    if (!result)
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open csv file " +
                QString(Constants::getDatasetDataFilename()) + ".");
        return result;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Csv file " + Constants::getDatasetDataFilename() + " opened.");

    QTextStream stream(&zipFile);
    stream.setCodec("UTF-8");

    unsigned int lastEmittedPercent{0};
    int lineCounter{0};
    while (!stream.atEnd() && lineCounter < dataContainer.size())
    {
        if (fillSamplesOnly && lineCounter >= SAMPLE_SIZE)
            break;

        QStringList line{stream.readLine().split(';')};
        int columnToFill{0};
        for (int i = 0; i < columnCount(); ++i)
        {
            const QString& element = line.at(i);

            // If column is not active do nothing.
            if (!fillSamplesOnly && !activeColumns_[i])
                continue;

            addElementToContainer(getColumnFormat(i), element, dataContainer,
                                  lineCounter, columnToFill);
            columnToFill++;
        }
        lineCounter++;
        if (!fillSamplesOnly)
            updateProgress(lineCounter, rowCount(), lastEmittedPercent);
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Loaded " + QString::number(dataContainer->size()) + " rows.");

    zipFile.close();

    if (!fillSamplesOnly)
        rebuildDefinitonUsingActiveColumnsOnly();

    return true;
}

void DatasetInner::updateProgress(unsigned int currentRow,
                                  unsigned int rowCount,
                                  unsigned int& lastEmittedPercent)
{
    const unsigned int currentPercent{
        static_cast<unsigned int>(100. * (currentRow + 1) / rowCount)};
    if (currentPercent > lastEmittedPercent)
    {
        Q_EMIT loadingPercentChanged(currentPercent);
        lastEmittedPercent = currentPercent;
        QCoreApplication::processEvents();
    }
}

void DatasetInner::addElementToContainer(
    const ColumnType columnFormat, const QString& element,
    QVector<QVector<QVariant> >& dataContainer, const int lineCounter,
    const int columnToFill) const
{
    if (element.isEmpty())
    {
        dataContainer[lineCounter][columnToFill] =
            getDefaultVariantForFormat(columnFormat);
    }
    else
    {
        switch (columnFormat)
        {
            case ColumnType::NUMBER:
            {
                dataContainer[lineCounter][columnToFill] =
                    QVariant(element.toDouble());
                break;
            }

            case ColumnType::STRING:
            {
                dataContainer[lineCounter][columnToFill] =
                    QVariant(element.toInt());
                break;
            }

            case ColumnType::DATE:
            {
                dataContainer[lineCounter][columnToFill] =
                    QVariant(QDate::fromJulianDay(element.toInt()));
                break;
            }

            case ColumnType::UNKNOWN:
            {
                Q_ASSERT(false);
                dataContainer[lineCounter][columnToFill] =
                    QVariant(QVariant::String);
                break;
            }
        }
    }
}
