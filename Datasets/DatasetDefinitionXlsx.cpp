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

bool DatasetDefinitionXlsx::getColumnTypes(QuaZip& zip,
                                           const QString& sheetPath)
{
    QFile file(zip.getZipName());
    ImportXlsx importXlsx(file);

    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::ANALYSING);
    ProgressBarInfinite bar(barTitle, nullptr);
    bar.showDetached();
    bar.start();
    QTime performanceTimer;
    performanceTimer.start();
    QApplication::processEvents();

    bool success{false};
    const QString& sheetName = sheetToFileMapInZip_.key(sheetPath);
    std::tie(success, columnTypes_) = importXlsx.getColumnTypes(sheetName);
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
        return false;
    }

    rowsCount_ = static_cast<int>(importXlsx.getRowCount(sheetName).second);

    LOG(LogTypes::IMPORT_EXPORT,
        "Analysed file having " + QString::number(rowsCount_) +
            " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    return true;
}

bool DatasetDefinitionXlsx::getDataFromZip(
    QuaZip& zip, const QString& sheetPath,
    QVector<QVector<QVariant> >* dataContainer, bool fillSamplesOnly)
{
    QFile file(zip.getZipName());
    ImportXlsx importXlsx(file);

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

    bool success{false};
    const QString& sheetName = sheetToFileMapInZip_.key(sheetPath);
    if (fillSamplesOnly)
    {
        std::tie(success, *dataContainer) = importXlsx.getLimitedData(
            sheetName, {}, SAMPLE_SIZE < rowsCount_ ? SAMPLE_SIZE : rowsCount_);
    }
    else
    {
        QVector<unsigned int> excludedColumns;
        for (int i = 0; i < columnsCount_; ++i)
        {
            if (!activeColumns_.at(i))
                excludedColumns.append(i);
        }
        std::tie(success, *dataContainer) =
            importXlsx.getData(sheetName, excludedColumns);
    }

    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
        return false;
    }

    if (!fillSamplesOnly)
    {
        Q_ASSERT(rowsCount_ == dataContainer->size());

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
    if (!loadSharedStrings(importXlsx))
    {
        error_ = QObject::tr("File ") + zip.getZipName() +
                 QObject::tr(" is damaged.");
        return false;
    }

    return true;
}
