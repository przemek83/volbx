#include "ExportData.h"

#include <ProgressBarCounter.h>
#include <Qt5Quazip/quazipfile.h>
#include <QwtBleUtilities.h>
#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>

#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"
#include "Shared/Logger.h"

#include "DatasetUtilities.h"

namespace
{
constexpr char csvSeparator_{';'};

std::tuple<bool, int, QByteArray> saveDatasetDataFile(
    QuaZipFile& zipFile, const QAbstractItemView* view,
    QAbstractItemModel* proxyModel, ProgressBarCounter* bar)
{
    bool result =
        zipFile.open(QIODevice::WriteOnly,
                     QuaZipNewInfo(DatasetUtilities::getDatasetDataFilename()));
    if (!result)
    {
        LOG(LogTypes::IMPORT_EXPORT, "Error while saving data file.");
        return {false, 0, {}};
    }

    bool multiSelection =
        (QAbstractItemView::MultiSelection == view->selectionMode());
    QItemSelectionModel* selectionModel = view->selectionModel();

    int proxyRowCount = proxyModel->rowCount();
    int proxyColumnCount = proxyModel->columnCount();
    QHash<QString, int> stringsMap;
    int nextIndex = 1;
    constexpr char newLine = '\n';
    int rowCounter = 0;
    QByteArray stringsContent;
    unsigned int lastEmittedPercent{0};
    for (int i = 0; i < proxyRowCount; ++i)
    {
        if (multiSelection &&
            !selectionModel->isSelected(proxyModel->index(i, 0)))
        {
            continue;
        }

        for (int j = 0; j < proxyColumnCount; ++j)
        {
            const QVariant& actualField = proxyModel->index(i, j).data();

            if (!actualField.isNull())
            {
                switch (actualField.type())
                {
                    case QVariant::Double:
                    case QVariant::Int:
                    {
                        zipFile.write(actualField.toByteArray());
                        break;
                    }

                    case QVariant::Date:
                    case QVariant::DateTime:
                    {
                        zipFile.write(QByteArray::number(
                            actualField.toDate().toJulianDay()));
                        break;
                    }

                    case QVariant::String:
                    {
                        QString tmpString(actualField.toString());
                        int& index = stringsMap[tmpString];
                        if (0 == index)
                        {
                            index = nextIndex;
                            tmpString.replace(newLine, QLatin1String("\t"));
                            // No new line for first string.
                            if (1 != nextIndex)
                            {
                                stringsContent.append(newLine);
                            }
                            stringsContent.append(tmpString);
                            nextIndex++;
                        }
                        zipFile.write(QByteArray::number(index));

                        break;
                    }

                    default:
                    {
                        Q_ASSERT(false);
                        break;
                    }
                }
            }

            if (j != proxyColumnCount - 1)
            {
                zipFile.write(QByteArray(1, csvSeparator_));
            }
        }

        zipFile.write(QByteArray(1, newLine));
        rowCounter++;

        if (bar != nullptr)
        {
            const unsigned int currentPercent{static_cast<unsigned int>(
                100. * (rowCounter + 1) / proxyRowCount)};
            if (currentPercent > lastEmittedPercent)
            {
                bar->updateProgress(currentPercent);
                lastEmittedPercent = currentPercent;
                QCoreApplication::processEvents();
            }
        }
    }

    zipFile.close();

    return {true, rowCounter, stringsContent};
}

bool saveDatasetStringsFile(QuaZipFile& zipFile,
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

bool saveDatasetDefinitionFile(QuaZipFile& zipFile,
                               const QAbstractItemView* view, int rowCount)
{
    const TableModel* parentModel =
        (qobject_cast<FilteringProxyModel*>(view->model()))->getParentModel();

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
};  // namespace

namespace ExportUtilities
{
bool saveDataset(const QString& filePath, const QAbstractItemView* view,
                 ProgressBarCounter* bar)
{
    Q_ASSERT(view != nullptr);

    // Open archive.
    QuaZip zip(filePath);
    bool openSuccess = zip.open(QuaZip::mdCreate);
    if (!openSuccess)
        return false;

    // Data file, write directly in loop.
    // Only one zip file in archive can be accessed at a time.
    QuaZipFile zipFile(&zip);
    auto model = qobject_cast<QAbstractItemModel*>(view->model());
    Q_ASSERT(model != nullptr);

    auto [success, rowCount, stringsContent] =
        saveDatasetDataFile(zipFile, view, model, bar);

    if (!success)
        return false;

    success = saveDatasetStringsFile(zipFile, stringsContent);
    if (!success)
        return false;

    return saveDatasetDefinitionFile(zipFile, view, rowCount);
}

}  // namespace ExportUtilities
