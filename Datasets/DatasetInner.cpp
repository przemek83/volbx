#include "DatasetInner.h"

#include <Qt5Quazip/quazipfile.h>
#include <QApplication>
#include <QDir>
#include <QDomDocument>
#include <QTextStream>

#include "Common/DatasetUtilities.h"
#include "Shared/Logger.h"

DatasetInner::DatasetInner(const QString& name, QObject* parent)
    : Dataset(name, parent), datasetsDir_(DatasetUtilities::getDatasetsDir())
{
    zip_.setZipName(datasetsDir_ + name +
                    DatasetUtilities::getDatasetExtension());
}

bool DatasetInner::analyze()
{
    if (!zip_.open(QuaZip::mdUnzip))
        return false;

    QByteArray definitionContent;
    if (!loadXmlFile(definitionContent, zip_) || !fromXml(definitionContent) ||
        !loadStrings(zip_))
    {
        zip_.close();
        return false;
    }

    zip_.close();
    valid_ = true;

    return true;
}

std::tuple<bool, QVector<QVector<QVariant>>> DatasetInner::getSample()
{
    if (!zip_.open(QuaZip::mdUnzip))
        return {false, {}};

    auto [success, data] = fillData(zip_, true);
    if (!success)
    {
        zip_.close();
        return {false, {}};
    }

    updateSampleDataStrings(data);

    zip_.close();

    return {true, data};
}

std::tuple<bool, QVector<QVector<QVariant>>> DatasetInner::getAllData()
{
    if (!isValid())
        return {false, {}};

    if (!zip_.open(QuaZip::mdUnzip))
    {
        LOG(LogTypes::IMPORT_EXPORT, "Can not open file " + zip_.getZipName());
        return {false, {}};
    }

    QVector<QVector<QVariant>> data;
    std::tie(valid_, data) = fillData(zip_, false);
    zip_.close();

    return {true, data};
}

bool DatasetInner::fromXml(QByteArray& definitionContent)
{
    QDomDocument xmlDocument;

    // If parsing failure than exit.
    if (!xmlDocument.setContent(definitionContent))
    {
        LOG(LogTypes::IMPORT_EXPORT, "Xml file is corrupted.");
        return false;
    }

    LOG(LogTypes::IMPORT_EXPORT, "Read xml file:\n" + xmlDocument.toString());

    QDomElement root{xmlDocument.documentElement()};

    // Load columns elements.
    QDomNodeList columns{root.firstChildElement(XML_COLUMNS).childNodes()};

    LOG(LogTypes::IMPORT_EXPORT,
        "Read column count: " + QString::number(columns.count()));

    columnsCount_ = columns.size();
    for (unsigned int i = 0; i < columnsCount_; ++i)
    {
        QDomElement column{columns.at(i).toElement()};
        headerColumnNames_.push_back(column.attribute(XML_COLUMN_NAME));
        columnTypes_.push_back(static_cast<ColumnType>(
            column.attribute(XML_COLUMN_FORMAT).toInt()));

        QString special{column.attribute(XML_COLUMN_SPECIAL_TAG)};
        if (0 != special.compare(QLatin1String("")))
            setSpecialColumn(static_cast<SpecialColumn>(special.toInt()), i);
    }

    rowsCount_ =
        root.firstChildElement(XML_ROW_COUNT).attribute(XML_ROW_COUNT).toInt();

    return true;
}

bool DatasetInner::loadXmlFile(QByteArray& definitionContent, QuaZip& zip)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(DatasetUtilities::getDatasetDefinitionFilename());
    if (!zipFile.open(QIODevice::ReadOnly))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open xml file " +
                Constants::getDatasetDefinitionFilename() + ".");
        return false;
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
    zip.setCurrentFile(DatasetUtilities::getDatasetStringsFilename());

    if (!zipFile.open(QIODevice::ReadOnly))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open strings file " +
                Constants::getDatasetStringsFilename() + ".");
        return false;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Strings file " + Constants::getDatasetStringsFilename() + " opened.");

    QByteArray stringsContent{zipFile.readAll()};
    zipFile.close();

    QList<QByteArray> strings{stringsContent.split('\n')};
    // First element is empty.
    sharedStrings_.append(QVariant(QString()));
    for (const auto& currentString : strings)
        sharedStrings_.append(QVariant(QString::fromUtf8(currentString)));

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
    QVector<QVector<QVariant>>& dataContainer, const unsigned int lineCounter,
    const unsigned int columnToFill) const
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

QVariant DatasetInner::getDefaultVariantForFormat(const ColumnType format) const
{
    switch (format)
    {
        case ColumnType::STRING:
        {
            return QVariant(QVariant::Int);
        }

        case ColumnType::NUMBER:
        {
            return QVariant(QVariant::Double);
        }
        case ColumnType::DATE:
        {
            return QVariant(QVariant::Date);
        }

        case ColumnType::UNKNOWN:
        default:
        {
            Q_ASSERT(false);
            return QVariant(QVariant::String);
        }
    }
}

std::tuple<bool, QVector<QVector<QVariant>>> DatasetInner::fillData(
    QuaZip& zip, bool fillSamplesOnly)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(DatasetUtilities::getDatasetDataFilename());

    if (!zipFile.open(QIODevice::ReadOnly))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open csv file " +
                QString(Constants::getDatasetDataFilename()) + ".");
        return {false, {}};
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Csv file " + Constants::getDatasetDataFilename() + " opened.");

    QTextStream stream(&zipFile);
    stream.setCodec("UTF-8");

    unsigned int lastEmittedPercent{0};
    unsigned int lineCounter{0};
    QVector<QVector<QVariant>> data{prepareContainerForData(fillSamplesOnly)};
    while (!stream.atEnd() && lineCounter < rowCount())
    {
        if (fillSamplesOnly && lineCounter >= SAMPLE_SIZE)
            break;

        QStringList line{stream.readLine().split(';')};
        int columnToFill{0};
        for (unsigned int i = 0; i < columnCount(); ++i)
        {
            const QString& element{line.at(i)};

            // Do nothing if column is not active.
            if (!fillSamplesOnly && !activeColumns_[i])
                continue;

            addElementToContainer(getColumnFormat(i), element, data,
                                  lineCounter, columnToFill);
            columnToFill++;
        }
        lineCounter++;
        if (!fillSamplesOnly)
            updateProgress(lineCounter, rowCount(), lastEmittedPercent);
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Loaded " + QString::number(data->size()) + " rows.");

    zipFile.close();

    return {true, data};
}

QVector<QVector<QVariant>> DatasetInner::prepareContainerForData(
    bool fillSamplesOnly) const
{
    QVector<QVector<QVariant>> data;
    if (fillSamplesOnly)
    {
        const int rowsCountForSamples =
            (rowCount() > SAMPLE_SIZE ? SAMPLE_SIZE : rowCount());
        data.resize(rowsCountForSamples);
        for (int i = 0; i < rowsCountForSamples; ++i)
            data[i].resize(columnsCount_);
    }
    else
    {
        data.resize(rowCount());
        const int activeColumnsCount{activeColumns_.size()};
        for (int i = 0; i < data.size(); ++i)
            data[i].resize(activeColumnsCount);
    }
    return data;
}
