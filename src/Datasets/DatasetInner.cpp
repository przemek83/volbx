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
    columnsCount_ = static_cast<unsigned int>(columns.size());
    for (int column = 0; column < static_cast<int>(columnsCount_); ++column)
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
        root.firstChildElement(xmlRowCount_).attribute(xmlRowCount_).toUInt();

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

void DatasetInner::updateProgress(unsigned int currentRow,
                                  unsigned int rowCount,
                                  unsigned int& lastEmittedPercent)
{
    const unsigned int currentPercent{
        static_cast<unsigned int>((100. * (currentRow + 1)) / rowCount)};
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

            case ColumnType::UNKNOWN:
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

        case ColumnType::UNKNOWN:
            return QVariant(QMetaType(QMetaType::QString));
    }
    return QVariant(QMetaType(QMetaType::QString));
}

QVector<QVariant> DatasetInner::fillRow(const QStringList& line,
                                        bool fillSamplesOnly)
{
    QVector<QVariant> row;

    const int count{static_cast<int>(columnCount())};
    for (Column column{0}; column < count; ++column)
    {
        if ((!fillSamplesOnly) && (!activeColumns_[column]))
            continue;

        const QString& element{line.at(column)};
        row.append(getElementAsVariant(getColumnFormat(column), element));
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
    while ((!stream.atEnd()) && (lineCounter < rowCount()))
    {
        if (fillSamplesOnly && (lineCounter >= SAMPLE_SIZE))
            break;

        const QStringList line{stream.readLine().split(';')};
        data[static_cast<int>(lineCounter)] = fillRow(line, fillSamplesOnly);
        if (!fillSamplesOnly)
            updateProgress(lineCounter, rowCount(), lastEmittedPercent);
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
    data.resize(static_cast<int>(rowCount()));
    const qsizetype activeColumnsCount{activeColumns_.size()};
    for (auto& row : data)
        row.resize(activeColumnsCount);
    return data;
}

QVector<QVector<QVariant>> DatasetInner::prepareContainerForSampleData() const
{
    QVector<QVector<QVariant>> data;
    const int rowsCountForSamples{static_cast<int>(
        (rowCount() > SAMPLE_SIZE ? SAMPLE_SIZE : rowCount()))};
    data.resize(rowsCountForSamples);
    for (int i{0}; i < rowsCountForSamples; ++i)
        data[i].resize(static_cast<int>(columnsCount_));
    return data;
}
