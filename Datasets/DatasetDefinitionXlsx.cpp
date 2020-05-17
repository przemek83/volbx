#include "DatasetDefinitionXlsx.h"

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

DatasetDefinitionXlsx::DatasetDefinitionXlsx(const QString& name,
                                             QString& zipFileName)
    : DatasetDefinitionSpreadsheet(name, zipFileName)
{
    importer_ = std::make_unique<ImportXlsx>(zipFile_);
}

bool DatasetDefinitionXlsx::loadSharedStrings()
{
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

bool DatasetDefinitionXlsx::loadSpecificData()
{
    if (!loadSharedStrings())
    {
        error_ = QObject::tr("File ") + zipFile_.fileName() +
                 QObject::tr(" is damaged.");
        return false;
    }

    return true;
}
