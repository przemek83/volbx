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
    bool success{false};
    std::tie(success, sheetToFileMapInZip_) = importXlsx.getSheetList();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    return success;
}

bool DatasetDefinitionXlsx::loadStyles(ImportXlsx& importXlsx)
{
    bool success{false};
    std::tie(success, dateStyles_, allStyles_) = importXlsx.getStyles();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    return success;
}

bool DatasetDefinitionXlsx::loadSharedStrings(ImportXlsx& importXlsx)
{
    auto [success, sharedStringsSet] = importXlsx.getSharedStrings();
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
        return false;
    }
    for (const auto& sharedString : sharedStringsSet)
    {
        stringsMap_[sharedString] = nextIndex_;
        nextIndex_++;
    }
    return success;
}

bool DatasetDefinitionXlsx::getColumnList(QuaZip& zip, const QString& sheetName)
{
    // Loading column names is using Excel names names. Set 600 temporary.
    columnsCount_ = EibleUtilities::getMaxExcelColumns();
    excelColNames_ = EibleUtilities::generateExcelColumnNames(columnsCount_);

    if (zip.setCurrentFile(sheetName))
    {
        QuaZipFile zipFile(&zip);

        // Opening file.
        if (!zipFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            LOG(LogTypes::IMPORT_EXPORT,
                "Can not open file " + zipFile.getFileName() + ".");
            return false;
        }

        QXmlStreamReader xmlStreamReader;
        xmlStreamReader.setDevice(&zipFile);

        // Variable with actual type of data in cell (s, str, null).
        QString currentColType = QStringLiteral("s");

        // Go to first row.
        while (!xmlStreamReader.atEnd() &&
               xmlStreamReader.name() != "sheetData")
        {
            xmlStreamReader.readNext();
        }

        xmlStreamReader.readNext();
        xmlStreamReader.readNext();

        // Actual column number, from 0.
        int column = Constants::NOT_SET_COLUMN;
        QXmlStreamReader::TokenType lastToken = xmlStreamReader.tokenType();

        const QRegExp regExp(QLatin1String("[0-9]"));

        // Parse first row.
        while (!xmlStreamReader.atEnd() && xmlStreamReader.name() != "row")
        {
            // If we encounter start of cell content we add it to list.
            if (xmlStreamReader.name().toString() == QLatin1String("c") &&
                xmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
            {
                column++;
                QString rowNumber(xmlStreamReader.attributes()
                                      .value(QLatin1String("r"))
                                      .toString());

                // If cells are missing add default name.
                while (excelColNames_.indexOf(rowNumber.remove(regExp)) >
                       column)
                {
                    headerColumnNames_ << emptyColName_;
                    column++;
                }
                // Remember column type.
                currentColType = xmlStreamReader.attributes()
                                     .value(QLatin1String("t"))
                                     .toString();
            }

            // If we encounter start of cell content than add it to list.
            if (!xmlStreamReader.atEnd() &&
                xmlStreamReader.name().toString() == QLatin1String("v") &&
                xmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
            {
                if (currentColType == QLatin1String("s"))
                {
                    int value = xmlStreamReader.readElementText().toInt() + 1;
                    headerColumnNames_.push_back(stringsMap_.key(value));
                }
                else
                {
                    if (currentColType == QLatin1String("str"))
                    {
                        headerColumnNames_.push_back(
                            xmlStreamReader.readElementText());
                    }
                    else
                    {
                        headerColumnNames_.push_back(
                            xmlStreamReader.readElementText());
                    }
                }
            }

            // If we encounter empty cell than add it to list.
            if (xmlStreamReader.name().toString() == QLatin1String("c") &&
                xmlStreamReader.tokenType() == QXmlStreamReader::EndElement &&
                lastToken == QXmlStreamReader::StartElement)
            {
                headerColumnNames_ << emptyColName_;
            }
            lastToken = xmlStreamReader.tokenType();
            xmlStreamReader.readNext();
        }
    }
    else
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "File named " + sheetName + " not found in archive.");
        return false;
    }

    return true;
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

    columnsFormat_.clear();

    for (int i = 0; i < columnsCount_; ++i)
    {
        columnsFormat_.push_back(DATA_FORMAT_UNKNOWN);
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
            if (columnsFormat_.at(column) == DATA_FORMAT_UNKNOWN)
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
                            columnsFormat_[column] = DATA_FORMAT_STRING;
                        }
                    }
                    else
                    {
                        columnsFormat_[column] = DATA_FORMAT_STRING;
                    }
                }
                else
                {
                    QString otherValue =
                        xmlStreamAtrributes.value(sTag).toString();

                    if (!otherValue.isEmpty() &&
                        dateStyles_.contains(allStyles_.at(otherValue.toInt())))
                    {
                        columnsFormat_[column] = DATA_FORMAT_DATE;
                    }
                    else
                    {
                        columnsFormat_[column] = DATA_FORMAT_FLOAT;
                    }
                }
            }
            else
            {
                if (DATA_FORMAT_STRING != columnsFormat_.at(column))
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
                                columnsFormat_[column] = DATA_FORMAT_STRING;
                            }
                        }
                        else
                        {
                            columnsFormat_[column] = DATA_FORMAT_STRING;
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
                            if (columnsFormat_.at(column) != DATA_FORMAT_DATE)
                            {
                                columnsFormat_[column] = DATA_FORMAT_STRING;
                            }
                        }
                        else
                        {
                            if (columnsFormat_.at(column) != DATA_FORMAT_FLOAT)
                            {
                                columnsFormat_[column] = DATA_FORMAT_STRING;
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
        if (DATA_FORMAT_UNKNOWN == columnsFormat_.at(i))
        {
            columnsFormat_[i] = DATA_FORMAT_STRING;
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
                getDefaultVariantForFormat(columnsFormat_[i]);
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
            DataFormat format = columnsFormat_.at(column);

            switch (format)
            {
                case DATA_FORMAT_STRING:
                {
                    // Strings.
                    if (0 == currentColType.compare(sTag))
                    {
                        currentDataRow[activeColumnsMapping[column]] = QVariant(
                            xmlStreamReader.readElementText().toInt() + 1);
                    }
                    else
                    {
                        currentDataRow[activeColumnsMapping[column]] = QVariant(
                            getStringIndex(xmlStreamReader.readElementText()));
                    }
                    break;
                }

                case DATA_FORMAT_DATE:
                {
                    // If YYYY-MM-DD HH:MI:SS cut and left YYYY-MM-DD.
                    int daysToAdd = static_cast<int>(
                        xmlStreamReader.readElementText().toDouble());
                    currentDataRow[activeColumnsMapping[column]] =
                        QVariant(EibleUtilities::getStartOfExcelWorld().addDays(
                            daysToAdd));
                    break;
                }

                case DATA_FORMAT_FLOAT:
                {
                    if (0 != currentColType.compare(sTag))
                    {
                        currentDataRow[activeColumnsMapping[column]] = QVariant(
                            xmlStreamReader.readElementText().toDouble());
                    }

                    break;
                }

                case DATA_FORMAT_UNKNOWN:
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
