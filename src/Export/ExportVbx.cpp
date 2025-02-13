#include "ExportVbx.h"

#include <FilteringProxyModel.h>
#include <quazip/quazipfile.h>
#include <QAbstractItemView>
#include <QFile>
#include <QVariant>

#include <Common/DatasetUtilities.h>
#include <ModelsAndViews/TableModel.h>
#include <Shared/Logger.h>

bool ExportVbx::generateVbx(const QAbstractItemView& view, QIODevice& ioDevice)
{
    return exportView(view, ioDevice) && exportStrings(ioDevice) &&
           exportDefinition(view, ioDevice);
}

bool ExportVbx::writeContent(const QByteArray& content, QIODevice& ioDevice)
{
    return write(ioDevice, dataset_utilities::getDatasetDataFilename(), content,
                 QuaZip::mdCreate);
}

QByteArray ExportVbx::getEmptyContent() { return QByteArrayLiteral(""); }

QByteArray ExportVbx::generateHeaderContent(
    [[maybe_unused]] const QAbstractItemModel& model)
{
    return QByteArrayLiteral("");
}

QByteArray ExportVbx::generateRowContent(const QAbstractItemModel& model,
                                         int row,
                                         [[maybe_unused]] int skippedRowsCount)
{
    QByteArray rowContent;
    const int count{model.columnCount()};
    for (int column{0}; column < count; ++column)
    {
        if (const QVariant actualField{model.index(row, column).data()};
            !actualField.isNull())
            variantToString(actualField, rowContent, separator_);

        if (column != (count - 1))
            rowContent.append(separator_);
    }

    rowContent.append(QByteArrayLiteral("\n"));
    ++lines_;

    return rowContent;
}

QByteArray ExportVbx::getContentEnding() { return QByteArrayLiteral(""); }

bool ExportVbx::exportStrings(QIODevice& ioDevice) const
{
    return write(ioDevice, dataset_utilities::getDatasetStringsFilename(),
                 stringsContent_, QuaZip::mdAdd);
}

bool ExportVbx::exportDefinition(const QAbstractItemView& view,
                                 QIODevice& ioDevice) const
{
    const TableModel* parentModel{
        (::qobject_cast<FilteringProxyModel*>(view.model()))->getParentModel()};
    const QByteArray definitionContent{parentModel->definitionToXml(lines_)};

    return write(ioDevice, dataset_utilities::getDatasetDefinitionFilename(),
                 definitionContent, QuaZip::mdAdd);
}

void ExportVbx::variantToString(const QVariant& variant,
                                QByteArray& destinationArray,
                                [[maybe_unused]] char separator)
{
    switch (variant.typeId())
    {
        case QMetaType::Double:
        case QMetaType::Int:
        {
            destinationArray.append(variant.toByteArray());
            break;
        }

        case QMetaType::QDate:
        case QMetaType::QDateTime:
        {
            destinationArray.append(
                QByteArray::number(variant.toDate().toJulianDay()));
            break;
        }

        case QMetaType::QString:
        {
            const int index{ExportVbx::getStringIndex(variant)};
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
    if (!outZip.open(mode))
        return false;

    const QuaZipNewInfo info(fileName);
    if (QuaZipFile zipFile(&outZip);
        (!zipFile.open(QIODevice::WriteOnly, info)) ||
        (zipFile.write(data) == -1))
    {
        LOG(LogTypes::IMPORT_EXPORT,
            "Error while saving file " + fileName + ".");
        return false;
    }

    return true;
}

int ExportVbx::getStringIndex(const QVariant& variant)
{
    QString entry(variant.toString());
    int& index{stringsMap_[entry]};
    if (index == 0)
    {
        index = nextIndex_;
        entry.replace(newLine_, QStringLiteral("\t"));
        // No new line for first string.
        if (nextIndex_ != 1)
            stringsContent_.append(newLine_);
        stringsContent_.append(entry.toUtf8());
        ++nextIndex_;
    }

    return index;
}
