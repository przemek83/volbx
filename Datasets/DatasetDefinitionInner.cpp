#include "DatasetDefinitionInner.h"

#include <memory>

#include <ProgressBarCounter.h>
#include <quazip5/quazipfile.h>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>

#include "Common/Constants.h"
#include "Shared/Logger.h"

#include "DatasetInner.h"

DatasetDefinitionInner::DatasetDefinitionInner(const QString& name)
    : DatasetDefinition(name)
{
    zip_.setZipName(DatasetInner::getDatasetsDir() + name_ +
                    Constants::getDatasetExtension());
    valid_ = load();
}

bool DatasetDefinitionInner::isValid() const { return valid_; }

bool DatasetDefinitionInner::load()
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

    result = fillData(zip_, &sampleData_, true);

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

void DatasetDefinitionInner::updateSampleDataStrings()
{
    for (int i = 0; i < columnsCount_; ++i)
    {
        if (DATA_FORMAT_STRING == columnsFormat_.at(i))
        {
            for (auto& sampleDataRow : sampleData_)
            {
                sampleDataRow[i] =
                    stringsTable_[sampleDataRow[i].toULongLong()];
            }
        }
    }
}

bool DatasetDefinitionInner::loadXmlFile(QByteArray& definitionContent,
                                         QuaZip& zip)
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

bool DatasetDefinitionInner::fromXml(QByteArray& definitionContent)
{
    headerColumnNames_.clear();
    columnsFormat_.clear();
    specialColumns_.clear();

    QDomDocument xmlDocument(name_);

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
        columnsFormat_.push_back(static_cast<DataFormat>(
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

bool DatasetDefinitionInner::fillData(
    QuaZip& zip, QVector<QVector<QVariant> >* dataContainer,
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

    QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::LOADING);
    std::unique_ptr<ProgressBarCounter> bar =
        (fillSamplesOnly ? nullptr
                         : std::make_unique<ProgressBarCounter>(
                               barTitle, rowCount(), nullptr));
    if (bar != nullptr)
        bar->showDetached();

    QTime performanceTimer;
    performanceTimer.start();

    int lineCounter = 0;
    while (!stream.atEnd() && lineCounter < dataContainer->size())
    {
        if (fillSamplesOnly && lineCounter >= SAMPLE_SIZE)
        {
            break;
        }

        QStringList line = stream.readLine().split(';');

        int columnToFill = 0;
        for (int i = 0; i < columnCount(); ++i)
        {
            const QString& element = line.at(i);

            // If column is not active do nothing.
            if (!fillSamplesOnly && !activeColumns_[i])
            {
                continue;
            }

            addElementToContainer(getColumnFormat(i), element, dataContainer,
                                  lineCounter, columnToFill);

            columnToFill++;
        }

        lineCounter++;

        if (!fillSamplesOnly)
        {
            bar->updateProgress(lineCounter);
        }
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Loaded " + QString::number(dataContainer->size()) + " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    zipFile.close();

    if (!fillSamplesOnly)
    {
        rebuildDefinitonUsingActiveColumnsOnly();
    }

    return true;
}

void DatasetDefinitionInner::addElementToContainer(
    const DataFormat columnFormat, const QString& element,
    QVector<QVector<QVariant> >* dataContainer, const int lineCounter,
    const int columnToFill) const
{
    if (element.isEmpty())
    {
        (*dataContainer)[lineCounter][columnToFill] =
            getDefaultVariantForFormat(columnFormat);
    }
    else
    {
        switch (columnFormat)
        {
            case DATA_FORMAT_FLOAT:
            {
                (*dataContainer)[lineCounter][columnToFill] =
                    QVariant(element.toDouble());
                break;
            }

            case DATA_FORMAT_STRING:
            {
                (*dataContainer)[lineCounter][columnToFill] =
                    QVariant(element.toInt());
                break;
            }

            case DATA_FORMAT_DATE:
            {
                (*dataContainer)[lineCounter][columnToFill] =
                    QVariant(QDate::fromJulianDay(element.toInt()));
                break;
            }

            case DATA_FORMAT_UNKNOWN:
            {
                Q_ASSERT(false);
                (*dataContainer)[lineCounter][columnToFill] =
                    QVariant(QVariant::String);
                break;
            }
        }
    }
}

bool DatasetDefinitionInner::loadStrings(QuaZip& zip)
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

std::unique_ptr<QVariant[]> DatasetDefinitionInner::getSharedStringTable()
{
    return std::move(stringsTable_);
}

bool DatasetDefinitionInner::getData(QVector<QVector<QVariant> >* dataContainer)
{
    // Open archive.
    if (!zip_.open(QuaZip::mdUnzip))
    {
        LOG(LogTypes::IMPORT_EXPORT, "Can not open file " + zip_.getZipName());
        return false;
    }

    bool result = fillData(zip_, dataContainer, false);

    zip_.close();

    return result;
}
