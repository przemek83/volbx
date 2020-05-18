#include "DatasetXlsx.h"

#include <cmath>
#include <future>
#include <memory>

#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QFuture>
#include <QVariant>

#include "Common/Constants.h"
#include "Shared/Logger.h"

DatasetXlsx::DatasetXlsx(const QString& name, QString& zipFileName)
    : DatasetSpreadsheet(name, zipFileName)
{
    importer_ = std::make_unique<ImportXlsx>(zipFile_);
}

bool DatasetXlsx::loadSharedStrings()
{
    // TODO 17.05.2020 get rid of get() on smart pointer.
    auto [success, sharedStringsList] =
        dynamic_cast<ImportXlsx*>(importer_.get())->getSharedStrings();
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
        return false;
    }
    for (const auto& sharedString : sharedStringsList)
    {
        stringsMap_[sharedString] = nextSharedStringIndex_;
        nextSharedStringIndex_++;
    }
    return success;
}

bool DatasetXlsx::loadSpecificData()
{
    if (!loadSharedStrings())
    {
        error_ = QObject::tr("File ") + zipFile_.fileName() +
                 QObject::tr(" is damaged.");
        return false;
    }

    return true;
}
