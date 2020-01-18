#include "DatasetDefinitionInner.h"

#include <memory>

#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <quazip5/quazipfile.h>

#include "Common/Constants.h"
#include "Common/ProgressBar.h"
#include "Shared/Logger.h"

#include "DatasetInner.h"

DatasetDefinitionInner::DatasetDefinitionInner(const QString& name)
    : DatasetDefinition(name)
{
    zip_.setZipName(DatasetInner::getDatasetsDir() +
                    name_ +
                    Constants::datasetExtension);
    valid_ = load();
}

bool DatasetDefinitionInner::isValid() const
{
    return valid_;
}

bool DatasetDefinitionInner::load()
{
    //Open archive.
    bool result = zip_.open(QuaZip::mdUnzip);
    if (!result)
    {
        return result;
    }

    QByteArray definitionContent;

    result = loadXmlFile(definitionContent, zip_);

    if (!result)
    {
        zip_.close();
        return result;
    }

    result = fromXml(definitionContent);

    if (!result)
    {
        zip_.close();
        return result;
    }

    result = loadStrings(zip_);
    if (!result)
    {
        zip_.close();
        return result;
    }

    int rowsCountForSamples = (rowCount() > sampleSize_ ? sampleSize_ : rowCount());
    sampleData_.resize(rowsCountForSamples);

    for (int i = 0 ; i < rowsCountForSamples ; ++i)
    {
        sampleData_[i].resize(columnsCount_);
    }

    result = fillData(zip_, &sampleData_, true);

    if (!result)
    {
        zip_.close();
        return result;
    }

    //Set proper strings for sample data.
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
            for (int j = 0; j < sampleData_.size(); ++j)
            {
                sampleData_[j][i] = stringsTable_[sampleData_[j][i].toInt()];
            }
        }
    }
}

bool DatasetDefinitionInner::loadXmlFile(QByteArray& definitionContent, QuaZip& zip)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(Constants::datasetDefinitionFilename);

    bool result = zipFile.open(QIODevice::ReadOnly);
    if (!result)
    {
        LOG(LOG_IMPORT_EXPORT,
            "Can not open xml file " +
            QString(Constants::datasetDefinitionFilename_) + ".");
        return result;
    }

    LOG(LOG_IMPORT_EXPORT,
        "Xml file " +  QString(Constants::datasetDefinitionFilename_) + " opened.");

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

    //If parsing failure than exit.
    if (!xmlDocument.setContent(definitionContent))
    {
        LOG(LOG_IMPORT_EXPORT, "Xml file is corrupted.");
        return false;
    }

    LOG(LOG_IMPORT_EXPORT, "Read xml file:\n" + xmlDocument.toString());

    QDomElement root = xmlDocument.documentElement();

    //Load columns elements.
    QDomNodeList columns =
        root.firstChildElement(datasetDefinitionXmlNames_[DATASET_COLUMNS]).childNodes();

    LOG(LOG_IMPORT_EXPORT, "Read column count: " + QString::number(columns.count()));

    columnsCount_ = columns.size();

    for (int i = 0; i < columnsCount_; ++i)
    {
        QDomElement column = columns.at(i).toElement();
        headerColumnNames_.push_back(
            column.attribute(datasetDefinitionXmlNames_[DATASET_COLUMN_NAME]));
        static const QString columnFormatTag =
            datasetDefinitionXmlNames_[DATASET_COLUMN_FORMAT];
        columnsFormat_.push_back(
            static_cast<DataFormat>(column.attribute(columnFormatTag).toInt()));

        QString special =
            column.attribute(datasetDefinitionXmlNames_[DATASET_COLUMN_SPECIAL_TAG]);
        if (0 != special.compare(QLatin1String("")))
        {
            specialColumns_[static_cast<SpecialColumn>(special.toInt())] = i;
        }
    }

    //Read row count.
    static const QString rowCountTag(datasetDefinitionXmlNames_[DATASET_ROW_COUNT]);
    rowsCount_ = root.firstChildElement(rowCountTag).attribute(rowCountTag).toInt();

    return true;
}

bool DatasetDefinitionInner::fillData(QuaZip& zip,
                                      QVector<QVector<QVariant> >* dataContainer,
                                      bool fillSamplesOnly)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(Constants::datasetDataFilename);

    bool result = zipFile.open(QIODevice::ReadOnly);
    if (!result)
    {
        LOG(LOG_IMPORT_EXPORT,
            "Can not open csv file " +
            QString(Constants::datasetDataFilename_) + ".");
        return result;
    }

    LOG(LOG_IMPORT_EXPORT,
        "Csv file " + QString(Constants::datasetDataFilename_) +
        " opened.");

    QTextStream stream(&zipFile);
    stream.setCodec("UTF-8");

    std::unique_ptr<ProgressBar> bar {fillSamplesOnly ? nullptr : std::make_unique<ProgressBar>(ProgressBar::PROGRESS_TITLE_LOADING, rowCount(), nullptr)};

    QTime performanceTimer;
    performanceTimer.start();

    int lineCounter = 0;
    while (!stream.atEnd() && lineCounter < dataContainer->size())
    {
        if (fillSamplesOnly && lineCounter >= sampleSize_)
        {
            break;
        }

        QStringList line = stream.readLine().split(';');

        int columnToFill = 0;
        for (int i = 0; i < columnCount(); ++i)
        {
            QString element = line.at(i);

            //If column is not active do nothing.
            if (!fillSamplesOnly && !activeColumns_[i])
            {
                continue;
            }

            addElementToContainer(getColumnFormat(i),
                                  element,
                                  dataContainer,
                                  lineCounter,
                                  columnToFill);

            columnToFill++;
        }

        lineCounter++;

        if (!fillSamplesOnly)
        {
            bar->updateProgress(lineCounter);
        }
    }

    LOG(LOG_IMPORT_EXPORT, "Loaded " + QString::number(dataContainer->size()) +
        " rows in time " +
        QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
        " seconds.");

    zipFile.close();

    if (!fillSamplesOnly)
    {
        rebuildDefinitonUsingActiveColumnsOnly();
    }

    return true;
}

void
DatasetDefinitionInner::addElementToContainer(const DataFormat columnFormat,
                                              const QString& element,
                                              QVector<QVector<QVariant> >* dataContainer,
                                              const int lineCounter,
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
    zip.setCurrentFile(Constants::datasetStringsFilename);

    bool result = zipFile.open(QIODevice::ReadOnly);
    if (!result)
    {
        LOG(LOG_IMPORT_EXPORT,
            "Can not open strings file " +
            QString(Constants::datasetStringsFilename_) + ".");
        return result;
    }

    LOG(LOG_IMPORT_EXPORT,
        "Strings file " + QString(Constants::datasetStringsFilename_) + " opened.");

    QByteArray stringsContent = zipFile.readAll();
    zipFile.close();

    QList<QByteArray> strings = stringsContent.split('\n');
    //First element is empty.
    stringsTable_ = std::make_unique<QVariant[]>(strings.size() + 1);
    stringsTable_[0] = QVariant(QString());
    size_t counter = 1;
    for (const auto& currentString : strings)
    {
        stringsTable_[counter] = QVariant(QString(currentString));
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
    //Open archive.
    if (!zip_.open(QuaZip::mdUnzip))
    {
        LOG(LOG_IMPORT_EXPORT, "Can not open file " + zip_.getZipName());
        return false;
    }

    bool result = fillData(zip_, dataContainer, false);

    zip_.close();

    return result;
}
