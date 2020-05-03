#include "DatasetDefinitionOds.h"

#include <future>
#include <memory>

#include <ProgressBarCounter.h>
#include <ProgressBarInfinite.h>
#include <quazip5/quazipfile.h>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QXmlStreamReader>

#include "Common/Constants.h"
#include "Shared/Logger.h"

DatasetDefinitionOds::DatasetDefinitionOds(const QString& name,
                                           const QString& zipFileName)
    : DatasetDefinitionSpreadsheet(name, zipFileName),
      odsFile_(zipFileName),
      importOds_(odsFile_)
{
    importOds_.setNameForEmptyColumn(QObject::tr("no name"));
}

bool DatasetDefinitionOds::getSheetList([[maybe_unused]] QuaZip& zip)
{
    auto [success, sheetNames] = importOds_.getSheetNames();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    sheetNames_ = std::move(sheetNames);
    return success;
}

bool DatasetDefinitionOds::getColumnList([[maybe_unused]] QuaZip& zip,
                                         const QString& sheetName)
{
    auto [success, columnNames] = importOds_.getColumnNames(sheetName);
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    headerColumnNames_ = std::move(columnNames);
    return success;
}

bool DatasetDefinitionOds::openZipAndMoveToSecondRow(
    QuaZip& zip, const QString& sheetName, QuaZipFile& zipFile,
    QXmlStreamReader& xmlStreamReader)
{
    // Open file in zip archive.
    if (!zip.setCurrentFile(QStringLiteral("content.xml")))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open file " + sheetName + " in archive.");
        return false;
    }
    zipFile.setZip(&zip);
    if (!zipFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Can not open file " + zipFile.getFileName() + ".");
        return false;
    }

    xmlStreamReader.setDevice(&zipFile);

    // Move to first row in selected sheet.
    while (!xmlStreamReader.atEnd() &&
           xmlStreamReader.name() != "table:table" &&
           xmlStreamReader.attributes().value(QLatin1String("table:name")) !=
               sheetName)
    {
        xmlStreamReader.readNext();
    }

    bool secondRow = false;
    while (!xmlStreamReader.atEnd())
    {
        if (xmlStreamReader.name() == "table-row" &&
            xmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
        {
            if (secondRow)
            {
                break;
            }
            secondRow = true;
        }
        xmlStreamReader.readNext();
    }

    return true;
}

bool DatasetDefinitionOds::getColumnTypes([[maybe_unused]] QuaZip& zip,
                                          const QString& sheetName)
{
    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::ANALYSING);
    ProgressBarInfinite bar(barTitle, nullptr);
    bar.showDetached();
    bar.start();
    QTime performanceTimer;
    performanceTimer.start();
    QApplication::processEvents();

    bool success{false};

    auto futureColumnTypes =
        std::async(&ImportOds::getColumnTypes, &importOds_, sheetName);
    std::chrono::milliseconds span(1);
    while (futureColumnTypes.wait_for(span) == std::future_status::timeout)
        QCoreApplication::processEvents();
    std::tie(success, columnTypes_) = futureColumnTypes.get();
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
        return false;
    }

    rowsCount_ = static_cast<int>(importOds_.getRowCount(sheetName).second);

    LOG(LogTypes::IMPORT_EXPORT,
        "Analysed file having " + QString::number(rowsCount_) +
            " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    return true;
}

bool DatasetDefinitionOds::getDataFromZip(
    QuaZip& zip, const QString& sheetName,
    QVector<QVector<QVariant> >* dataContainer, bool fillSamplesOnly)
{
    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::LOADING);
    std::unique_ptr<ProgressBarCounter> bar =
        (fillSamplesOnly ? nullptr
                         : std::make_unique<ProgressBarCounter>(
                               barTitle, rowsCount_, nullptr));
    if (bar != nullptr)
        bar->showDetached();

    QTime performanceTimer;
    performanceTimer.start();

    QuaZipFile zipFile;
    QXmlStreamReader xmlStreamReader;

    if (!openZipAndMoveToSecondRow(zip, sheetName, zipFile, xmlStreamReader))
    {
        return false;
    }

    // Null elements row.
    QVector<QVariant> templateDataRow;

    QMap<int, int> activeColumnsMapping;

    int columnToFill = 0;

    templateDataRow.resize(
        (fillSamplesOnly ? columnsCount_ : getActiveColumnCount()));
    for (int i = 0; i < columnsCount_; ++i)
    {
        if (fillSamplesOnly || activeColumns_.at(i))
        {
            templateDataRow[columnToFill] =
                getDefaultVariantForFormat(columnTypes_[i]);

            activeColumnsMapping[i] = columnToFill;
            columnToFill++;
        }
    }

    // Protection from potential core related to empty rows.
    int containerSize = dataContainer->size();
    for (int i = 0; i < containerSize; ++i)
    {
        (*dataContainer)[i] = templateDataRow;
    }

    // Current row data.
    QVector<QVariant> currentDataRow(templateDataRow);

    // Actual column number.
    int column = Constants::NOT_SET_COLUMN;

    // Actual data type in cell (s, str, null).
    QString currentColType = QStringLiteral("string");

    // Current row number.
    int rowCounter = 0;

    int repeatCount = 1;

    int cellsFilledInRow = 0;

    // Optimization.
    const QString tableTag(QStringLiteral("table"));
    const QString tableRowTag(QStringLiteral("table-row"));
    const QString tableCellTag(QStringLiteral("table-cell"));
    const QString officeValueTypeTag(QStringLiteral("office:value-type"));
    const QString columnsRepeatedTag(
        QStringLiteral("table:number-columns-repeated"));
    const QString pTag(QStringLiteral("p"));
    const QString officeDateValueTag(QStringLiteral("office:date-value"));
    const QString officeValueTag(QStringLiteral("office:value"));
    const QString dateFormat(QStringLiteral("yyyy-MM-dd"));

    const QString emptyString(QLatin1String(""));

    while (!xmlStreamReader.atEnd() &&
           0 != xmlStreamReader.name().compare(tableTag) &&
           rowCounter < rowsCount_)
    {
        // If start of row encountered than reset column counter add
        // increment row counter.
        if (0 == xmlStreamReader.name().compare(tableRowTag) &&
            xmlStreamReader.isStartElement())
        {
            column = Constants::NOT_SET_COLUMN;

            if (0 != cellsFilledInRow)
            {
                (*dataContainer)[rowCounter] = currentDataRow;
                currentDataRow = QVector<QVariant>(templateDataRow);
                cellsFilledInRow = 0;
                rowCounter++;

                if (!fillSamplesOnly)
                {
                    bar->updateProgress(rowCounter);
                }

                if (fillSamplesOnly && rowCounter >= SAMPLE_SIZE)
                {
                    break;
                }
            }
        }

        // When we encounter start of cell description.
        if (0 == xmlStreamReader.name().compare(tableCellTag) &&
            xmlStreamReader.isStartElement())
        {
            column++;

            // If we encounter column outside expected grid we move to row end.
            if (column >= columnsCount_)
            {
                while (!xmlStreamReader.atEnd() &&
                       !(0 == xmlStreamReader.name().compare(tableRowTag) &&
                         xmlStreamReader.isEndElement()))
                {
                    xmlStreamReader.readNext();
                }
                continue;
            }

            // Remember cell type.
            currentColType = xmlStreamReader.attributes()
                                 .value(officeValueTypeTag)
                                 .toString();

            // Number of repeats.
            repeatCount = xmlStreamReader.attributes()
                              .value(columnsRepeatedTag)
                              .toString()
                              .toInt();

            if (0 == repeatCount)
            {
                repeatCount = 1;
            }

            if (column + repeatCount - 1 >= columnsCount_)
            {
                repeatCount = columnsCount_ - column;
            }

            if (!currentColType.isEmpty())
            {
                QVariant value;
                ColumnType format = columnTypes_.at(column);

                switch (format)
                {
                    case ColumnType::STRING:
                    {
                        while (!xmlStreamReader.atEnd() &&
                               0 != xmlStreamReader.name().compare(pTag))

                        {
                            xmlStreamReader.readNext();
                        }

                        while (xmlStreamReader.tokenType() !=
                                   QXmlStreamReader::Characters &&
                               0 !=
                                   xmlStreamReader.name().compare(tableCellTag))
                        {
                            xmlStreamReader.readNext();
                        }

                        const QString* stringPointer =
                            xmlStreamReader.text().string();
                        value = QVariant(getStringIndex(
                            (stringPointer == nullptr ? emptyString
                                                      : *stringPointer)));

                        break;
                    }

                    case ColumnType::DATE:
                    {
                        static const int odsStringDateLength{10};
                        value = QVariant(
                            QDate::fromString(xmlStreamReader.attributes()
                                                  .value(officeDateValueTag)
                                                  .toString()
                                                  .left(odsStringDateLength),
                                              dateFormat));

                        break;
                    }

                    case ColumnType::NUMBER:
                    {
                        value = QVariant(xmlStreamReader.attributes()
                                             .value(officeValueTag)
                                             .toDouble());
                        break;
                    }

                    case ColumnType::UNKNOWN:
                    {
                        Q_ASSERT(false);
                        break;
                    }
                }

                for (int i = 0; i < repeatCount; ++i)
                {
                    if (!fillSamplesOnly && !activeColumns_[column + i])
                    {
                        continue;
                    }

                    cellsFilledInRow++;
                    currentDataRow[activeColumnsMapping[column + i]] = value;
                }
            }

            column += repeatCount - 1;
        }
        xmlStreamReader.readNextStartElement();
    }

    if (!fillSamplesOnly)
    {
        Q_ASSERT(rowsCount_ == dataContainer->size());

        LOG(LogTypes::IMPORT_EXPORT,
            "Read file having " + QString::number(rowsCount_) +
                " rows in time " +
                QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
                " seconds.");
    }

    return true;
}

const QString& DatasetDefinitionOds::getSheetName()
{
    return sheetNames_.constFirst();
}

bool DatasetDefinitionOds::loadSpecificData(QuaZip& /*zip*/)
{
    // Nothing specific for .ods.
    return true;
}
