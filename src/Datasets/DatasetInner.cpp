#include "DatasetInner.h"

#include <quazip/quazipfile.h>
#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QTextStream>

#include <DatasetUtilities.h>
#include <Logger.h>

DatasetInner::DatasetInner(const QString& name) : Dataset{name}
{
    zip_.setZipName(datasetsDir_ + name +
                    dataset_utilities::getDatasetExtension());
}

bool DatasetInner::analyze()
{
    if (!openZip())
        return false;

    if (QByteArray definitionContent; (!loadXmlFile(definitionContent, zip_)) ||
                                      (!fromXml(definitionContent)) ||
                                      (!loadStrings(zip_)))
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
    auto [success, data]{fillData(zip_, true)};
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
    const QDomNodeList columns{
        root.firstChildElement(xmlColumns_).childNodes()};
    LOG(LogTypes::IMPORT_EXPORT,
        "Read column count: " + QString::number(columns.count()));
    columnsCount_ = columns.size();
    for (int column = 0; column < columnsCount_; ++column)
    {
        const QDomElement columnElement{columns.at(column).toElement()};
        headerColumnNames_.push_back(columnElement.attribute(xmlColumnName_));
        columnTypes_.push_back(static_cast<ColumnType>(
            columnElement.attribute(xmlColumnFormat_).toInt()));
        checkForTaggedColumn(columnElement, column);
    }
}

void DatasetInner::checkForTaggedColumn(const QDomElement& columnElement,
                                        Column column)
{
    if (const QString tag{columnElement.attribute(xmlColumnTag_)};
        !tag.isEmpty())
    {
        setTaggedColumn(static_cast<ColumnTag>(tag.toInt()), column);
    }
    else
    {
        // Deprecated Tag used before 10.09.2020. Kept for compatibility.
        const QString columnTagDeprecated{
            columnElement.attribute(xmlColumnTagDeprecated_)};
        if (!columnTagDeprecated.isEmpty())
            setTaggedColumn(static_cast<ColumnTag>(columnTagDeprecated.toInt()),
                            column);
    }
}

bool DatasetInner::fromXml(const QByteArray& definitionContent)
{
    QDomDocument xmlDocument;
    if (!xmlDocument.setContent(definitionContent))
    {
        LOG(LogTypes::IMPORT_EXPORT, QStringLiteral("XML file is corrupted."));
        return false;
    }

    LOG(LogTypes::IMPORT_EXPORT, "Read xml file:\n" + xmlDocument.toString());

    const QDomElement root{xmlDocument.documentElement()};
    retrieveColumnsFromXml(root);
    rowsCount_ =
        root.firstChildElement(xmlRowCount_).attribute(xmlRowCount_).toInt();

    return true;
}

bool DatasetInner::openQuaZipFile(QuaZipFile& zipFile)
{
    if (!zipFile.open(QIODevice::ReadOnly))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open file " + zipFile.getZip()->getCurrentFileName() +
                ".");
        return false;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "File " + zipFile.getZip()->getCurrentFileName() + " opened.");

    return true;
}

bool DatasetInner::loadXmlFile(QByteArray& definitionContent, QuaZip& zip)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(dataset_utilities::getDatasetDefinitionFilename());
    if (!openQuaZipFile(zipFile))
        return false;

    definitionContent = zipFile.readAll();
    return true;
}

bool DatasetInner::loadStrings(QuaZip& zip)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(dataset_utilities::getDatasetStringsFilename());
    if (!openQuaZipFile(zipFile))
        return false;

    const QList<QByteArray> strings{zipFile.readAll().split('\n')};

    // First element need to be empty.
    sharedStrings_.append(QVariant(QString()));
    for (const auto& currentString : strings)
        sharedStrings_.append(QVariant(QString::fromUtf8(currentString)));

    return true;
}

void DatasetInner::updateProgress(int currentRow, int& lastEmittedPercent)
{
    const int currentPercent{
        static_cast<int>((100. * (currentRow + 1)) / rowCount())};
    if (currentPercent > lastEmittedPercent)
    {
        emit loadingPercentChanged(currentPercent);
        lastEmittedPercent = currentPercent;
        QCoreApplication::processEvents();
    }
}

QVariant DatasetInner::getElementAsVariant(ColumnType columnFormat,
                                           const QString& element)
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

            default:
                Q_ASSERT(false);
                elementAsVariant = QVariant(QMetaType(QMetaType::QString));
                break;
        }
    }
    return elementAsVariant;
}

QVariant DatasetInner::getDefaultVariantForFormat(ColumnType format)
{
    switch (format)
    {
        case ColumnType::STRING:
            return QVariant(QMetaType(QMetaType::Int));

        case ColumnType::NUMBER:
            return QVariant(QMetaType(QMetaType::Double));

        case ColumnType::DATE:
            return QVariant(QMetaType(QMetaType::QDate));

        default:
            return QVariant(QMetaType(QMetaType::QString));
    }
    return QVariant(QMetaType(QMetaType::QString));
}

QVector<QVariant> DatasetInner::fillRow(const QStringList& line,
                                        bool fillSamplesOnly) const
{
    QVector<QVariant> row;

    const int count{columnCount()};
    for (Column column{0}; column < count; ++column)
    {
        if (fillSamplesOnly || activeColumns_.at(column))
        {
            const QString& element{line.at(column)};
            row.append(getElementAsVariant(getColumnFormat(column), element));
        }
    }
    return row;
}

QVector<QVector<QVariant>> DatasetInner::parseData(QTextStream& stream,
                                                   bool fillSamplesOnly)
{
    int lastEmittedPercent{0};
    int lineCounter{0};

    QVector<QVector<QVariant>> data;
    if (fillSamplesOnly)
        data = prepareContainerForSampleData();
    else
        data = prepareContainerForAllData();

    while ((!stream.atEnd()) && (lineCounter < rowCount()))
    {
        if (fillSamplesOnly && (lineCounter >= SAMPLE_SIZE))
            return data;

        const QStringList line{stream.readLine().split(';')};
        data[lineCounter] = fillRow(line, fillSamplesOnly);
        if (!fillSamplesOnly)
            updateProgress(lineCounter, lastEmittedPercent);
        ++lineCounter;
    }

    return data;
}

std::tuple<bool, QVector<QVector<QVariant>>> DatasetInner::fillData(
    QuaZip& zip, bool fillSamplesOnly)
{
    QuaZipFile zipFile(&zip);
    zip.setCurrentFile(dataset_utilities::getDatasetDataFilename());
    if (!openQuaZipFile(zipFile))
        return {false, {}};

    QTextStream stream(&zipFile);
    stream.setEncoding(QStringConverter::Utf8);
    const QVector<QVector<QVariant>> data{parseData(stream, fillSamplesOnly)};
    LOG(LogTypes::IMPORT_EXPORT,
        "Loaded " + QString::number(data.size()) + " rows.");

    return {true, data};
}

QVector<QVector<QVariant>> DatasetInner::prepareContainerForAllData() const
{
    QVector<QVector<QVariant>> data;
    data.resize(rowCount());
    const qsizetype activeColumnsCount{activeColumns_.size()};
    for (auto& row : data)
        row.resize(activeColumnsCount);
    return data;
}

QVector<QVector<QVariant>> DatasetInner::prepareContainerForSampleData() const
{
    QVector<QVector<QVariant>> data;
    int rowsCountForSamples{rowCount()};
    if (rowsCountForSamples > SAMPLE_SIZE)
        rowsCountForSamples = SAMPLE_SIZE;

    data.resize(rowsCountForSamples);
    for (int i{0}; i < rowsCountForSamples; ++i)
        data[i].resize(columnsCount_);
    return data;
}
