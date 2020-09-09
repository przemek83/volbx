#include "DatasetXlsx.h"

#include <ImportXlsx.h>
#include <Logger.h>

DatasetXlsx::DatasetXlsx(const QString& name, const QString& zipFileName,
                         QObject* parent)
    : DatasetSpreadsheet(name, zipFileName, parent)
{
    importer_ = std::make_unique<ImportXlsx>(zipFile_);
}

bool DatasetXlsx::loadSharedStrings()
{
    const auto [success, sharedStringsList] =
        qobject_cast<ImportXlsx*>(importer_.get())->getSharedStrings();
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importer_->getLastError());
        return false;
    }
    for (const auto& sharedString : sharedStringsList)
        sharedStrings_.append(QVariant(sharedString));
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
