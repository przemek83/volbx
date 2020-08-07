#include "DatasetInner.h"

#include <Qt5Quazip/quazipfile.h>
#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QTextStream>

#include <DatasetUtilities.h>
#include <Logger.h>

DatasetInner::DatasetInner(const QString& name, QObject* parent)
    : Dataset(name, parent), datasetsDir_(DatasetUtilities::getDatasetsDir())
{
    zip_.setZipName(datasetsDir_ + name +
                    DatasetUtilities::getDatasetExtension());
}

bool DatasetInner::analyze()
{
    if (!openZip())
        return false;

    QByteArray definitionContent;
    if (!loadXmlFile(definitionContent, zip_) || !fromXml(definitionContent) ||
        !loadStrings(zip_))
        return false;

    valid_ = true;
    return true;
}

void DatasetInner::closeZip() { zip_.close(); }

bool DatasetInner::openZip()
{
    if (!zip_.open(QuaZip::mdUnzip))
    {
        LOG(LogTypes::IMPORT_EXPORT, "Can not open file " + zip_.getZipName());
        return false;
    }
    return true;
}

std::tuple<bool, QVector<QVector<QVariant>>> DatasetInner::getSample()
{
    auto [success, data] = fillData(zip_, true);
    if (!success)
        return {false, {}};

    updateSampleDataStrings(data);
    return {true, data};
}

std::tuple<bool, QVector<QVector<QVariant>>> DatasetInner::getAllData()
{
    if (!isValid())
        return {false, {}};

    QVector<QVector<QVariant>> data;
    std::tie(valid_, data) = fillData(zip_, false);

    return {true, data};
}

void DatasetInner::retrieveColumnsFromXml(const QDomElement& root)
{
    QDomNodeList columns{root.firstChildElement(XML_COLUMNS).childNodes()};
    LOG(LogTypes::IMPORT_EXPORT,
        "Read column count: " + QString::number(columns.count()));
    columnsCount_ = columns.size();
    for (unsigned int i = 0; i < columnsCount_; ++i)
    {
        const QDomElement column{columns.at(i).toElement()};
        headerColumnNames_.push_back(column.attribute(XML_COLUMN_NAME));
        columnTypes_.push_back(static_cast<ColumnType>(
            column.attribute(XML_COLUMN_FORMAT).toInt()));

        const QString columnTag{column.attribute(XML_COLUMN_TAG)};
        if (!columnTag.isEmpty())
            setTaggedColumn(static_cast<ColumnTag>(columnTag.toInt()), i);
    }
}

bool DatasetInner::fromXml(QByteArray& definitionContent)
{
    QDomDocument xmlDocument;
    if (!xmlDocument.setContent(definitionContent))
    {
        LOG(LogTypes::IMPORT_EXPORT, "XML file is corrupted.");
        return false;
    }

    LOG(LogTypes::IMPORT_EXPORT, "Read xml file:\n" + xmlDocument.toString());

    QDomElement root{xmlDocument.documentElement()};
    retrieveColumnsFromXml(root);
    rowsCount_ =
        root.firstChildElement(XML_ROW_COUNT).attribute(XML_ROW_COUNT).toInt();

    return true;
}

bool DatasetInner::openQuaZipFile(QuaZipFile& zipFile)
{
    if (!zipFile.open(QIODevice::ReadOnly))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open xml file " + zipFile.getZip()->getCurrentFileName() +
                ".");
        return false;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Xml file " + zipFile.getZip()->getCurrentFileName() + " opened.");

    return true;
}

bool DatasetInner::loadXmlFile(QByteArray& definitionContent, QuaZip& zip)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(DatasetUtilities::getDatasetDefinitionFilename());
    if (!openQuaZipFile(zipFile))
        return false;

    definitionContent = zipFile.readAll();
    return true;
}

bool DatasetInner::loadStrings(QuaZip& zip)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(DatasetUtilities::getDatasetStringsFilename());
    if (!openQuaZipFile(zipFile))
        return false;

    QList<QByteArray> strings{zipFile.readAll().split('\n')};

    // First element need to be empty.
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

QVariant DatasetInner::getElementAsVariant(ColumnType columnFormat,
                                           const QString& element) const
{
    QVariant elementAsVariant;
    if (element.isEmpty())
        elementAsVariant = getDefaultVariantForFormat(columnFormat);
    else
    {
        switch (columnFormat)
        {
            case ColumnType::NUMBER:
                elementAsVariant = QVariant(element.toDouble());
                break;

            case ColumnType::STRING:
                elementAsVariant = QVariant(element.toInt());
                break;

            case ColumnType::DATE:
                elementAsVariant =
                    QVariant(QDate::fromJulianDay(element.toInt()));
                break;

            case ColumnType::UNKNOWN:
                Q_ASSERT(false);
                elementAsVariant = QVariant(QVariant::String);
                break;
        }
    }
    return elementAsVariant;
}

QVariant DatasetInner::getDefaultVariantForFormat(const ColumnType format) const
{
    switch (format)
    {
        case ColumnType::STRING:
            return QVariant(QVariant::Int);

        case ColumnType::NUMBER:
            return QVariant(QVariant::Double);

        case ColumnType::DATE:
            return QVariant(QVariant::Date);

        case ColumnType::UNKNOWN:
        default:
        {
            Q_ASSERT(false);
            return QVariant(QVariant::String);
        }
    }
}

QVector<QVariant> DatasetInner::fillRow(const QStringList& line,
                                        bool fillSamplesOnly)
{
    QVector<QVariant> row;
    for (unsigned int i = 0; i < columnCount(); ++i)
    {
        if (!fillSamplesOnly && !activeColumns_[i])
            continue;

        const QString& element{line.at(i)};
        row.append(getElementAsVariant(getColumnFormat(i), element));
    }
    return row;
}

QVector<QVector<QVariant>> DatasetInner::parseData(QTextStream& stream,
                                                   bool fillSamplesOnly)
{
    unsigned int lastEmittedPercent{0};
    unsigned int lineCounter{0};
    QVector<QVector<QVariant>> data{fillSamplesOnly
                                        ? prepareContainerForSampleData()
                                        : prepareContainerForAllData()};
    while (!stream.atEnd() && lineCounter < rowCount())
    {
        if (fillSamplesOnly && lineCounter >= SAMPLE_SIZE)
            break;

        QStringList line{stream.readLine().split(';')};
        data[lineCounter] = fillRow(line, fillSamplesOnly);
        lineCounter++;
        if (!fillSamplesOnly)
            updateProgress(lineCounter, rowCount(), lastEmittedPercent);
    }
    return data;
}

std::tuple<bool, QVector<QVector<QVariant>>> DatasetInner::fillData(
    QuaZip& zip, bool fillSamplesOnly)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(DatasetUtilities::getDatasetDataFilename());
    if (!openQuaZipFile(zipFile))
        return {false, {}};

    QTextStream stream(&zipFile);
    stream.setCodec("UTF-8");
    QVector<QVector<QVariant>> data{parseData(stream, fillSamplesOnly)};
    LOG(LogTypes::IMPORT_EXPORT,
        "Loaded " + QString::number(data.size()) + " rows.");

    return {true, data};
}

QVector<QVector<QVariant>> DatasetInner::prepareContainerForAllData() const
{
    QVector<QVector<QVariant>> data;
    data.resize(rowCount());
    const int activeColumnsCount{activeColumns_.size()};
    for (int i = 0; i < data.size(); ++i)
        data[i].resize(activeColumnsCount);
    return data;
}

QVector<QVector<QVariant>> DatasetInner::prepareContainerForSampleData() const
{
    QVector<QVector<QVariant>> data;
    const int rowsCountForSamples =
        (rowCount() > SAMPLE_SIZE ? SAMPLE_SIZE : rowCount());
    data.resize(rowsCountForSamples);
    for (int i = 0; i < rowsCountForSamples; ++i)
        data[i].resize(columnsCount_);
    return data;
}
