#include "ExportVbx.h"

#include <FilteringProxyModel.h>
#include <Qt5Quazip/quazipfile.h>
#include <QAbstractItemView>
#include <QFile>
#include <QVariant>

#include <Common/DatasetUtilities.h>
#include <ModelsAndViews/TableModel.h>
#include <Shared/Logger.h>

ExportVbx::ExportVbx(QObject* parent) : ExportData(parent) {}

bool ExportVbx::generateVbx(const QAbstractItemView& view, QIODevice& ioDevice)
{
    return exportView(view, ioDevice) && exportStrings(ioDevice) &&
           exportDefinition(view, ioDevice);
}

bool ExportVbx::writeContent(const QByteArray& content, QIODevice& ioDevice)
{
    return write(ioDevice, DatasetUtilities::getDatasetDataFilename(), content,
                 QuaZip::mdCreate);
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

bool ExportVbx::exportStrings(QIODevice& ioDevice)
{
    return write(ioDevice, DatasetUtilities::getDatasetStringsFilename(),
                 stringsContent_, QuaZip::mdAdd);
}

bool ExportVbx::exportDefinition(const QAbstractItemView& view,
                                 QIODevice& ioDevice) const
{
    const TableModel* parentModel =
        (qobject_cast<FilteringProxyModel*>(view.model()))->getParentModel();
    QByteArray definitionContent{parentModel->definitionToXml(lines_)};

    return write(ioDevice, DatasetUtilities::getDatasetDefinitionFilename(),
                 definitionContent, QuaZip::mdAdd);
}

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
            if (index == 0)
            {
                index = nextIndex_;
                tmpString.replace(newLine_, QLatin1String("\t"));
                // No new line for first string.
                if (nextIndex_ != 1)
                    stringsContent_.append(newLine_);
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

bool ExportVbx::write(QIODevice& ioDevice, const QString& fileName,
                      const QByteArray& data, QuaZip::Mode mode)
{
    QuaZip outZip(&ioDevice);
    bool openSuccess = outZip.open(mode);
    if (!openSuccess)
        return false;

    QuaZipFile zipFile(&outZip);
    bool result = zipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName));
    if (!result || zipFile.write(data) == -1)
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Error while saving file " + fileName + ".");
        return false;
    }

    return true;
}
