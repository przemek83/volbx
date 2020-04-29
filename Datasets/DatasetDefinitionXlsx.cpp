#include "DatasetDefinitionXlsx.h"

#include <cmath>
#include <memory>

#include <EibleUtilities.h>
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
    : DatasetDefinitionSpreadsheet(name, zipFileName),
      xlsxFile_(zipFileName),
      importXlsx_(xlsxFile_)
{
}

bool DatasetDefinitionXlsx::getSheetList([[maybe_unused]] QuaZip& zip)
{
    auto [success, sheets] = importXlsx_.getSheets();
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

bool DatasetDefinitionXlsx::getColumnList([[maybe_unused]] QuaZip& zip,
                                          const QString& sheetName)
{
    importXlsx_.setNameForEmptyColumn(QObject::tr("no name"));
    bool success{false};
    std::tie(success, headerColumnNames_) =
        importXlsx_.getColumnNames(sheetToFileMapInZip_.key(sheetName));
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
    return success;
}

bool DatasetDefinitionXlsx::getColumnTypes([[maybe_unused]] QuaZip& zip,
                                           const QString& sheetPath)
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
    const QString& sheetName = sheetToFileMapInZip_.key(sheetPath);
    std::tie(success, columnTypes_) = importXlsx_.getColumnTypes(sheetName);
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx.getError().second);
        return false;
    }

    rowsCount_ = static_cast<int>(importXlsx_.getRowCount(sheetName).second);

    LOG(LogTypes::IMPORT_EXPORT,
        "Analysed file having " + QString::number(rowsCount_) +
            " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    return true;
}

bool DatasetDefinitionXlsx::getDataFromZip(
    [[maybe_unused]] QuaZip& zip, const QString& sheetPath,
    QVector<QVector<QVariant> >* dataContainer, bool fillSamplesOnly)
{
    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::LOADING);
    std::unique_ptr<ProgressBarCounter> bar =
        (fillSamplesOnly
             ? nullptr
             : std::make_unique<ProgressBarCounter>(barTitle, 100, nullptr));
    if (bar != nullptr)
    {
        QObject::connect(&importXlsx_,
                         &ImportSpreadsheet::progressPercentChanged, &(*bar),
                         &ProgressBarCounter::updateProgress);
        bar->showDetached();
    }

    QApplication::processEvents();

    QTime performanceTimer;
    performanceTimer.start();

    bool success{false};
    const QString& sheetName = sheetToFileMapInZip_.key(sheetPath);
    if (fillSamplesOnly)
    {
        std::tie(success, *dataContainer) = importXlsx_.getLimitedData(
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
            importXlsx_.getData(sheetName, excludedColumns);
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
    if (!loadSharedStrings(importXlsx_))
    {
        error_ = QObject::tr("File ") + zip.getZipName() +
                 QObject::tr(" is damaged.");
        return false;
    }

    return true;
}
