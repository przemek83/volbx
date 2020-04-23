#include "DatasetDefinitionXlsx.h"

#include <cmath>
#include <memory>

#include <EibleUtilities.h>
#include <ImportXlsx.h>
#include <ProgressBarCounter.h>
#include <ProgressBarInfinite.h>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QVariant>

#include "Common/Constants.h"
#include "Shared/Logger.h"

DatasetDefinitionXlsx::DatasetDefinitionXlsx(const QString& name,
                                             QString& zipFileName)
    : DatasetDefinitionSpreadsheet(name, zipFileName)
{
}

bool DatasetDefinitionXlsx::getSheetList(QuaZip& zip)
{
    QFile file(zip.getZipName());
    ImportXlsx importXlsx(file);
    auto [success, sheets] = importXlsx.getSheets();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    for (const auto& [sheetName, sheetPath] : sheets)
        sheetToFileMapInZip_[sheetName] = sheetPath;
    return success;
}

bool DatasetDefinitionXlsx::loadStyles(ImportXlsx& importXlsx)
{
    bool success{false};
    std::tie(success, dateStyles_) = importXlsx.getDateStyles();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    std::tie(success, allStyles_) = importXlsx.getAllStyles();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    return success;
}

bool DatasetDefinitionXlsx::loadSharedStrings(ImportXlsx& importXlsx)
{
    auto [success, sharedStringsList] = importXlsx.getSharedStrings();
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
        return false;
    }
    for (const auto& sharedString : sharedStringsList)
    {
        stringsMap_[sharedString] = nextSharedStringIndex_;
        nextSharedStringIndex_++;
    }
    return success;
}

bool DatasetDefinitionXlsx::getColumnList(QuaZip& zip, const QString& sheetName)
{
    QFile file(zip.getZipName());
    ImportXlsx importXlsx(file);
    importXlsx.setNameForEmptyColumn(QObject::tr("no name"));
    bool success{false};
    std::tie(success, headerColumnNames_) =
        importXlsx.getColumnNames(sheetToFileMapInZip_.key(sheetName));
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    return success;
}

bool DatasetDefinitionXlsx::openZipAndMoveToSecondRow(
    QuaZip& zip, const QString& sheetName, QuaZipFile& zipFile,
    QXmlStreamReader& xmlStreamReader)
{
    // Open archive.
    if (!zip.setCurrentFile(sheetName))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "File named " + sheetName + " not found in archive.");
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

    bool secondRow = false;
    while (!xmlStreamReader.atEnd())
    {
        if (xmlStreamReader.name() == "row" &&
            xmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
        {
            if (secondRow)
            {
                break;
            }
            secondRow = true;
        }
        xmlStreamReader.readNextStartElement();
    }

    return true;
}

bool DatasetDefinitionXlsx::getColumnTypes(QuaZip& zip,
                                           const QString& sheetName)
{
    excelColNames_ = EibleUtilities::generateExcelColumnNames(columnsCount_);

    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::ANALYSING);
    ProgressBarInfinite bar(barTitle, nullptr);
    bar.showDetached();
    bar.start();

    QTime performanceTimer;
    performanceTimer.start();

    QApplication::processEvents();

    QuaZipFile zipFile;
    QXmlStreamReader xmlStreamReader;

    if (!openZipAndMoveToSecondRow(zip, sheetName, zipFile, xmlStreamReader))
    {
        return false;
    }

    columnTypes_.clear();

    for (int i = 0; i < columnsCount_; ++i)
    {
        columnTypes_.push_back(ColumnType::UNKNOWN);
    }

    // Current column.
    int column = Constants::NOT_SET_COLUMN;

    // Current row.
    int rowCounter = 0;

    int charsToChopFromEndInCellName = 1;

    QXmlStreamAttributes xmlStreamAtrributes;

    // Optimization.
    const QString rowTag(QStringLiteral("row"));
    const QString cellTag(QStringLiteral("c"));
    const QString sheetDataTag(QStringLiteral("sheetData"));
    const QString sTag(QStringLiteral("s"));
    const QString strTag(QStringLiteral("str"));
    const QString rTag(QStringLiteral("r"));
    const QString tTag(QStringLiteral("t"));
    const QString vTag(QStringLiteral("v"));

    while (!xmlStreamReader.atEnd() &&
           0 != xmlStreamReader.name().compare(sheetDataTag))
    {
        // If start of row encountered than reset column counter add
        // increment row counter.
        if (0 == xmlStreamReader.name().compare(rowTag) &&
            xmlStreamReader.isStartElement())
        {
            column = Constants::NOT_SET_COLUMN;

            const int batchSize{100};
            if (0 == rowCounter % batchSize)
            {
                QApplication::processEvents();
            }

            rowCounter++;

            double power = pow(DECIMAL_BASE, charsToChopFromEndInCellName);
            if (power <= rowCounter + 1)
            {
                charsToChopFromEndInCellName++;
            }
        }

        // When we encounter start of cell description.
        if (0 == xmlStreamReader.name().compare(cellTag) &&
            xmlStreamReader.isStartElement())
        {
            column++;

            QString stringToChop =
                xmlStreamReader.attributes().value(rTag).toString();
            int numberOfCharsToRemove =
                stringToChop.size() - charsToChopFromEndInCellName;
            int expectedIndexCurrentColumn = excelColNames_.indexOf(
                stringToChop.left(numberOfCharsToRemove));

            // If cells are missing increment column number.
            while (expectedIndexCurrentColumn > column)
            {
                column++;
            }

            // If we encounter column outside expected grid we move to row end.
            if (expectedIndexCurrentColumn == Constants::NOT_SET_COLUMN)
            {
                xmlStreamReader.skipCurrentElement();
                continue;
            }

            // If data format in column is unknown than read it.
            if (columnTypes_.at(column) == ColumnType::UNKNOWN)
            {
                xmlStreamAtrributes = xmlStreamReader.attributes();
                QString value = xmlStreamAtrributes.value(tTag).toString();
                bool valueIsSTag = (0 == value.compare(sTag));
                if (valueIsSTag || 0 == value.compare(strTag))
                {
                    if (valueIsSTag)
                    {
                        xmlStreamReader.readNextStartElement();

                        if (0 == xmlStreamReader.name().compare(vTag) &&
                            xmlStreamReader.tokenType() ==
                                QXmlStreamReader::StartElement &&
                            stringsMap_
                                .key(xmlStreamReader.readElementText().toInt() +
                                     1)
                                .isEmpty())
                        {
                        }
                        else
                        {
                            columnTypes_[column] = ColumnType::STRING;
                        }
                    }
                    else
                    {
                        columnTypes_[column] = ColumnType::STRING;
                    }
                }
                else
                {
                    QString otherValue =
                        xmlStreamAtrributes.value(sTag).toString();

                    if (!otherValue.isEmpty() &&
                        dateStyles_.contains(allStyles_.at(otherValue.toInt())))
                    {
                        columnTypes_[column] = ColumnType::DATE;
                    }
                    else
                    {
                        columnTypes_[column] = ColumnType::NUMBER;
                    }
                }
            }
            else
            {
                if (ColumnType::STRING != columnTypes_.at(column))
                {
                    // If type of column is known than check if it is correct.
                    xmlStreamAtrributes = xmlStreamReader.attributes();
                    QString value = xmlStreamAtrributes.value(tTag).toString();
                    bool valueIsSTag = (0 == value.compare(sTag));
                    if (valueIsSTag || 0 == value.compare(strTag))
                    {
                        if (valueIsSTag)
                        {
                            xmlStreamReader.readNextStartElement();

                            if (0 == xmlStreamReader.name().compare(vTag) &&
                                xmlStreamReader.tokenType() ==
                                    QXmlStreamReader::StartElement &&
                                stringsMap_
                                    .key(xmlStreamReader.readElementText()
                                             .toInt() +
                                         1)
                                    .isEmpty())
                            {
                            }
                            else
                            {
                                columnTypes_[column] = ColumnType::STRING;
                            }
                        }
                        else
                        {
                            columnTypes_[column] = ColumnType::STRING;
                        }
                    }
                    else
                    {
                        QString othervalue =
                            xmlStreamAtrributes.value(sTag).toString();

                        if (!othervalue.isEmpty() &&
                            dateStyles_.contains(
                                allStyles_.at(othervalue.toInt())))
                        {
                            if (columnTypes_.at(column) != ColumnType::DATE)
                            {
                                columnTypes_[column] = ColumnType::STRING;
                            }
                        }
                        else
                        {
                            if (columnTypes_.at(column) != ColumnType::NUMBER)
                            {
                                columnTypes_[column] = ColumnType::STRING;
                            }
                        }
                    }
                }
            }
        }

        xmlStreamReader.readNextStartElement();
    }

    for (int i = 0; i < columnsCount_; ++i)
    {
        if (ColumnType::UNKNOWN == columnTypes_.at(i))
        {
            columnTypes_[i] = ColumnType::STRING;
        }
    }

    rowsCount_ = rowCounter;

    LOG(LogTypes::IMPORT_EXPORT,
        "Analyzed file having " + QString::number(rowsCount_) +
            " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    return true;
}

bool DatasetDefinitionXlsx::getDataFromZip(
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

    QApplication::processEvents();

    QTime performanceTimer;
    performanceTimer.start();

    QuaZipFile zipFile;
    QXmlStreamReader xmlStreamReader;

    openZipAndMoveToSecondRow(zip, sheetName, zipFile, xmlStreamReader);

    // Actual column number.
    int column = Constants::NOT_SET_COLUMN;

    // Actual data type in cell (s, str, null).
    QString currentColType = QStringLiteral("s");

    // Actual row number.
    int rowCounter = 0;

    // Null elements row.
    QVector<QVariant> templateDataRow;

    QMap<int, int> activeColumnsMapping = QMap<int, int>();

    int columnToFill = 0;

    templateDataRow.resize(fillSamplesOnly ? columnsCount_
                                           : getActiveColumnCount());
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

    // Nmebr of chars to chop from end.
    int charsToChopFromEndInCellName = 1;

    // Optimization.
    const QString rowTag(QStringLiteral("row"));
    const QString cellTag(QStringLiteral("c"));
    const QString sheetDataTag(QStringLiteral("sheetData"));
    const QString sTag(QStringLiteral("s"));
    const QString vTag(QStringLiteral("v"));
    const QString rTag(QStringLiteral("r"));
    const QString tTag(QStringLiteral("t"));

    while (!xmlStreamReader.atEnd() &&
           0 != xmlStreamReader.name().compare(sheetDataTag) &&
           rowCounter <= rowsCount_)
    {
        // If start of row encountered than reset column counter add
        // increment row counter.
        if (0 == xmlStreamReader.name().compare(rowTag) &&
            xmlStreamReader.isStartElement())
        {
            column = Constants::NOT_SET_COLUMN;

            if (0 != rowCounter)
            {
                (*dataContainer)[rowCounter - 1] = currentDataRow;
                currentDataRow = QVector<QVariant>(templateDataRow);

                double power = pow(DECIMAL_BASE, charsToChopFromEndInCellName);
                if (qRound(power) <= rowCounter + 2)
                {
                    charsToChopFromEndInCellName++;
                }
            }

            rowCounter++;

            if (!fillSamplesOnly)
            {
                bar->updateProgress(rowCounter);
            }

            if (fillSamplesOnly && rowCounter > containerSize)
            {
                break;
            }
        }

        // When we encounter start of cell description.
        if (0 == xmlStreamReader.name().compare(cellTag) &&
            xmlStreamReader.isStartElement())
        {
            column++;

            QString stringToChop =
                xmlStreamReader.attributes().value(rTag).toString();
            int expectedIndexCurrentColumn =
                excelColNames_.indexOf(stringToChop.left(
                    stringToChop.size() - charsToChopFromEndInCellName));

            // If cells missing than increment column number.
            while (expectedIndexCurrentColumn > column)
            {
                column++;
            }

            // If we encounter column outside expected grid we move to row end.
            if (expectedIndexCurrentColumn == Constants::NOT_SET_COLUMN)
            {
                xmlStreamReader.skipCurrentElement();
                continue;
            }

            // Remember cell type.
            currentColType =
                xmlStreamReader.attributes().value(tTag).toString();
        }

        // Insert data into table.
        if (!xmlStreamReader.atEnd() &&
            0 == xmlStreamReader.name().compare(vTag) &&
            xmlStreamReader.tokenType() == QXmlStreamReader::StartElement &&
            (fillSamplesOnly || activeColumns_.at(column)))
        {
            ColumnType format = columnTypes_.at(column);

            switch (format)
            {
                case ColumnType::STRING:
                {
                    // Strings.
                    if (0 == currentColType.compare(sTag))
                    {
                        currentDataRow[activeColumnsMapping[column]] =
                            QVariant(xmlStreamReader.readElementText().toInt());
                    }
                    else
                    {
                        currentDataRow[activeColumnsMapping[column]] = QVariant(
                            getStringIndex(xmlStreamReader.readElementText()));
                    }
                    break;
                }

                case ColumnType::DATE:
                {
                    // If YYYY-MM-DD HH:MI:SS cut and left YYYY-MM-DD.
                    int daysToAdd = static_cast<int>(
                        xmlStreamReader.readElementText().toDouble());
                    currentDataRow[activeColumnsMapping[column]] =
                        QVariant(EibleUtilities::getStartOfExcelWorld().addDays(
                            daysToAdd));
                    break;
                }

                case ColumnType::NUMBER:
                {
                    if (0 != currentColType.compare(sTag))
                    {
                        currentDataRow[activeColumnsMapping[column]] = QVariant(
                            xmlStreamReader.readElementText().toDouble());
                    }

                    break;
                }

                case ColumnType::UNKNOWN:
                {
                    Q_ASSERT(false);
                    break;
                }
            }
        }
        xmlStreamReader.readNextStartElement();
    }

    if (0 != rowCounter && (!fillSamplesOnly || SAMPLE_SIZE > rowsCount_))
    {
        Q_ASSERT(rowCounter <= rowsCount_);
        if (rowCounter <= rowsCount_)
        {
            (*dataContainer)[rowCounter - 1] = currentDataRow;
        }
    }

    if (!fillSamplesOnly)
    {
        Q_ASSERT(rowsCount_ == containerSize);

        rebuildDefinitonUsingActiveColumnsOnly();

        LOG(LogTypes::IMPORT_EXPORT,
            "Loaded file having " + QString::number(rowsCount_) +
                " rows in time " +
                QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
                " seconds.");
    }

    return true;
}

const QString& DatasetDefinitionXlsx::getSheetName()
{
    return sheetToFileMapInZip_.constBegin().value();
}

bool DatasetDefinitionXlsx::loadSpecificData(QuaZip& zip)
{
    QFile file(zip.getZipName());
    ImportXlsx importXlsx(file);
    if (!loadStyles(importXlsx) || !loadSharedStrings(importXlsx))
    {
        error_ = QObject::tr("File ") + zip.getZipName() +
                 QObject::tr(" is damaged.");
        return false;
    }

    return true;
}
