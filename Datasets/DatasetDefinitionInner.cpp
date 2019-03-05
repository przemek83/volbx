#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QApplication>
#include <QDebug>
#include <quazip5/quazipfile.h>
#include <memory>

#include "Common/Constants.h"
#include "Shared/Logger.h"
#include "Common/ProgressBar.h"

#include "DatasetDefinitionInner.h"
#include "DatasetInner.h"

const char* DatasetDefinitionInner::datasetDefinitionXmlNames_[] =
{
    "DATASET",
    "COLUMNS",
    "COLUMN",
    "NAME",
    "FORMAT",
    "SPECIAL_TAG",
    "ROW_COUNT"
};


DatasetDefinitionInner::DatasetDefinitionInner(QString name)
    : DatasetDefinition(name),
      stringsTable_(nullptr)
{
    zip_.setZipName(DatasetInner::getDatasetsDir() +
                    name_ +
                    Constants::datasetExtension_);
    valid_ = load();
}

DatasetDefinitionInner::~DatasetDefinitionInner()
{
    delete[] stringsTable_;

    //QuaZip destructor checks if zip is still open.
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
    zip.setCurrentFile(Constants::datasetDefinitionFilename_);

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

    QDomDocument xmlDocument(__FUNCTION__);

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
        if (0 != special.compare(""))
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
    zip.setCurrentFile(Constants::datasetDataFilename_);

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

    std::unique_ptr<ProgressBar> bar;

    if (!fillSamplesOnly)
    {
        bar.reset(new ProgressBar(ProgressBar::PROGRESS_TITLE_LOADING, rowCount(), nullptr));
    }

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
    zip.setCurrentFile(Constants::datasetStringsFilename_);

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
    //First element is empty element.
    stringsTable_ = new QVariant[strings.size() + 1];
    stringsTable_[0] = QVariant(QString());
    int counter = 1;
    Q_FOREACH (QByteArray currentString, strings)
    {
        stringsTable_[counter] = QVariant(QString(currentString));
        counter++;
    }

    return true;
}

QVariant* DatasetDefinitionInner::getSharedStringTable()
{
    ///Manage memory only when it was not retrieved.
    QVariant* temp = stringsTable_;
    stringsTable_ = nullptr;
    return temp;
}

void DatasetDefinitionInner::toXml(QByteArray& data, int rowCountNumber) const
{
    QDomDocument xmlDocument(datasetDefinitionXmlNames_[DATASET_NAME]);
    QDomElement root =
        xmlDocument.createElement(datasetDefinitionXmlNames_[DATASET_NAME]);
    xmlDocument.appendChild(root);

    //Columns.
    QDomElement columns =
        xmlDocument.createElement(datasetDefinitionXmlNames_[DATASET_COLUMNS]);
    root.appendChild(columns);

    for (int i = 0; i < columnsCount_; ++i)
    {
        QDomElement node =
            xmlDocument.createElement(datasetDefinitionXmlNames_[DATASET_COLUMN]);
        node.setAttribute(datasetDefinitionXmlNames_[DATASET_COLUMN_NAME],
                          headerColumnNames_.at(i));
        node.setAttribute(datasetDefinitionXmlNames_[DATASET_COLUMN_FORMAT],
                          columnsFormat_.at(i));

        QMapIterator<SpecialColumn, int> it(specialColumns_);
        if (it.findNext(i))
        {
            node.setAttribute(datasetDefinitionXmlNames_[DATASET_COLUMN_SPECIAL_TAG],
                              QString::number(static_cast<int>(it.key())));
        }
        columns.appendChild(node);
    }

    //Add row count.
    QDomElement rowCount =
        xmlDocument.createElement(datasetDefinitionXmlNames_[DATASET_ROW_COUNT]);
    rowCount.setAttribute(datasetDefinitionXmlNames_[DATASET_ROW_COUNT],
                          QString::number(rowCountNumber));
    root.appendChild(rowCount);

    data = xmlDocument.toByteArray();
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
