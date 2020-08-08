#include "ExportVbx.h"

#include <FilteringProxyModel.h>
#include <Qt5Quazip/quazipfile.h>
#include <QAbstractItemView>
#include <QCoreApplication>
#include <QFile>
#include <QVariant>

#include <Common/DatasetUtilities.h>
#include <ModelsAndViews/TableModel.h>
#include <Shared/Logger.h>

ExportVbx::ExportVbx(QObject* parent) : ExportData(parent) {}

bool ExportVbx::generateVbx(const QAbstractItemView& view, QIODevice& ioDevice)
{
    if (!exportView(view, ioDevice))
        return false;

    QuaZip outZip(&ioDevice);
    bool openSuccess = outZip.open(QuaZip::mdAdd);
    if (!openSuccess)
        return false;

    QuaZipFile zipFile(&outZip);

    if (!exportStrings(zipFile, stringsContent_))
        return false;

    if (!exportDefinition(zipFile, view, lines_))
        return false;

    return true;
}

bool ExportVbx::writeContent(const QByteArray& content, QIODevice& ioDevice)
{
    // Create out xlsx.
    QuaZip outZip(&ioDevice);
    bool openSuccess = outZip.open(QuaZip::mdCreate);
    if (!openSuccess)
        return false;

    QuaZipFile zipFile(&outZip);

    bool result =
        zipFile.open(QIODevice::WriteOnly,
                     QuaZipNewInfo(DatasetUtilities::getDatasetDataFilename()));
    if (!result || zipFile.write(content) == -1)
    {
        LOG(LogTypes::IMPORT_EXPORT, "Error while saving definition file.");
        return false;
    }
    zipFile.close();

    return true;
}

QByteArray ExportVbx::getEmptyContent() { return QByteArrayLiteral(""); }

QByteArray ExportVbx::generateHeaderContent([
    [maybe_unused]] const QAbstractItemModel& model)
{
    return QByteArrayLiteral("");
}

QByteArray ExportVbx::generateRowContent(const QAbstractItemModel& model,
                                         int row,
                                         [[maybe_unused]] int skippedRowsCount)
{
    QByteArray rowContent;
    for (int j = 0; j < model.columnCount(); ++j)
    {
        QVariant actualField = model.index(row, j).data();
        if (!actualField.isNull())
            variantToString(actualField, rowContent, separator_);
        if (j != model.columnCount() - 1)
            rowContent.append(separator_);
    }

    rowContent.append(QByteArrayLiteral("\n"));
    lines_++;

    return rowContent;
}

QByteArray ExportVbx::getContentEnding() { return QByteArrayLiteral(""); }

void ExportVbx::variantToString(const QVariant& variant,
                                QByteArray& destinationArray,
                                [[maybe_unused]] char separator)
{
    switch (variant.type())
    {
        case QVariant::Double:
        case QVariant::Int:
        {
            destinationArray.append(variant.toByteArray());
            break;
        }

        case QVariant::Date:
        case QVariant::DateTime:
        {
            destinationArray.append(
                QByteArray::number(variant.toDate().toJulianDay()));
            break;
        }

        case QVariant::String:
        {
            QString tmpString(variant.toString());
            int& index = stringsMap_[tmpString];
            if (0 == index)
            {
                index = nextIndex_;
                tmpString.replace(newLine_, QLatin1String("\t"));
                // No new line for first string.
                if (1 != nextIndex_)
                {
                    stringsContent_.append(newLine_);
                }
                stringsContent_.append(tmpString);
                nextIndex_++;
            }
            destinationArray.append(QByteArray::number(index));
            break;
        }

        default:
        {
            Q_ASSERT(false);
            break;
        }
    }
}

bool ExportVbx::exportStrings(QuaZipFile& zipFile,
                              const QByteArray& stringsContent)
{
    bool result = zipFile.open(
        QIODevice::WriteOnly,
        QuaZipNewInfo(DatasetUtilities::getDatasetStringsFilename()));
    if (!result || zipFile.write(stringsContent) == -1)
    {
        LOG(LogTypes::IMPORT_EXPORT, "Error while saving strings file.");
        return false;
    }
    zipFile.close();

    return true;
}

bool ExportVbx::exportDefinition(QuaZipFile& zipFile,
                                 const QAbstractItemView& view, int rowCount)
{
    const TableModel* parentModel =
        (qobject_cast<FilteringProxyModel*>(view.model()))->getParentModel();

    QByteArray definitionContent{parentModel->definitionToXml(rowCount)};

    bool result = zipFile.open(
        QIODevice::WriteOnly,
        QuaZipNewInfo(DatasetUtilities::getDatasetDefinitionFilename()));
    if (!result || zipFile.write(definitionContent) == -1)
    {
        LOG(LogTypes::IMPORT_EXPORT, "Error while saving definition file.");
        return false;
    }
    zipFile.close();

    return true;
}
