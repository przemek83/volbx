#include "ExportData.h"

#include <ProgressBarCounter.h>
#include <QwtBleUtilities.h>
#include <quazip5/quazipfile.h>
#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>

#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"
#include "Shared/Logger.h"

#include "Constants.h"

namespace
{
constexpr char csvSeparator_{';'};

constexpr char tsvSeparator_{'\t'};

void variantToString(const QVariant& variant, QByteArray* destinationArray,
                     char separator, bool innerFormat)
{
    switch (variant.type())
    {
        case QVariant::Double:
        case QVariant::Int:
        {
            if (innerFormat)
            {
                destinationArray->append(variant.toString());
            }
            else
            {
                QString value(QwtBleUtilities::doubleToStringUsingLocale(
                    variant.toDouble(), 2));
                destinationArray->append(value);
            }
            break;
        }

        case QVariant::Date:
        case QVariant::DateTime:
        {
            static const QString defDateFormat(
                QwtBleUtilities::getDefaultDateFormat());
            destinationArray->append(variant.toDate().toString(defDateFormat));
            break;
        }

        case QVariant::String:
        {
            QString tmpString(variant.toString());
            if (separator == tsvSeparator_)
            {
                // Simplification -> change tabs and new lines into spaces.
                tmpString = tmpString.replace(
                    QRegExp("[" + QString(tsvSeparator_) + "\n" + "]"),
                    QStringLiteral(" "));
                destinationArray->append(tmpString);
            }
            else
            {
                tmpString.replace('"', QStringLiteral("\"\""));
                destinationArray->append("\"" + tmpString + "\"");
            }

            break;
        }

        default:
        {
            Q_ASSERT(false);
            break;
        }
    }
}

void dataToByteArray(const QAbstractItemView* view,
                     QByteArray* destinationArray, char separator,
                     bool innerFormat)
{
    /*
     * Using standards described on:
     * http://en.wikipedia.org/wiki/Comma-separated_values
     * and
     * http://tools.ietf.org/html/rfc4180
     */

    auto proxyModel = qobject_cast<QAbstractItemModel*>(view->model());
    Q_ASSERT(nullptr != proxyModel);

    int proxyColumnCount = proxyModel->columnCount();

    // Save column names.
    if (!innerFormat)
    {
        for (int j = 0; j < proxyColumnCount; ++j)
        {
            destinationArray->append(
                proxyModel->headerData(j, Qt::Horizontal).toString());
            if (j != proxyColumnCount - 1)
            {
                destinationArray->append(separator);
            }
        }
        destinationArray->append("\n");
    }

    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::SAVING);
    ProgressBarCounter bar(barTitle, proxyModel->rowCount(), nullptr);
    bar.showDetached();

    bool multiSelection =
        (QAbstractItemView::MultiSelection == view->selectionMode());
    QItemSelectionModel* selectionModel = view->selectionModel();

    int proxyRowCount = proxyModel->rowCount();
    for (int i = 0; i < proxyRowCount; ++i)
    {
        if (multiSelection &&
            !selectionModel->isSelected(proxyModel->index(i, 0)))
        {
            continue;
        }

        for (int j = 0; j < proxyColumnCount; ++j)
        {
            QVariant actualField = proxyModel->index(i, j).data();

            if (!actualField.isNull())
            {
                variantToString(actualField, destinationArray, separator,
                                innerFormat);
            }

            if (j != proxyColumnCount - 1)
            {
                destinationArray->append(separator);
            }
        }
        destinationArray->append("\n");
        bar.updateProgress(i + 1);
    }
}

std::tuple<bool, int, QByteArray> saveDatasetDataFile(
    QuaZipFile& zipFile, const QAbstractItemView* view,
    QAbstractItemModel* proxyModel, ProgressBarCounter& bar)
{
    bool result =
        zipFile.open(QIODevice::WriteOnly,
                     QuaZipNewInfo(Constants::getDatasetDataFilename()));
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
    int rowCount = 0;
    QByteArray stringsContent;
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
        rowCount++;

        bar.updateProgress(i + 1);
    }

    zipFile.close();

    return {true, rowCount, stringsContent};
}

bool saveDatasetStringsFile(QuaZipFile& zipFile,
                            const QByteArray& stringsContent)
{
    bool result =
        zipFile.open(QIODevice::WriteOnly,
                     QuaZipNewInfo(Constants::getDatasetStringsFilename()));
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

    QByteArray definitionContent;
    parentModel->getDatasetDefinition()->toXml(definitionContent, rowCount);

    bool result =
        zipFile.open(QIODevice::WriteOnly,
                     QuaZipNewInfo(Constants::getDatasetDefinitionFilename()));
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
bool saveDataset(const QString& filePath, const QAbstractItemView* view)
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

    QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::SAVING);
    ProgressBarCounter bar(barTitle, model->rowCount(), nullptr);
    bar.showDetached();

    auto [success, rowCount, stringsContent] =
        saveDatasetDataFile(zipFile, view, model, bar);

    if (!success)
        return false;

    success = saveDatasetStringsFile(zipFile, stringsContent);
    if (!success)
        return false;

    return saveDatasetDefinitionFile(zipFile, view, rowCount);
}

void quickAsTSV(const QAbstractItemView* view)
{
    QByteArray content;
    dataToByteArray(view, &content, tsvSeparator_, false);

    QApplication::clipboard()->setText(QString::fromUtf8(content));
}

bool asCsv(const QAbstractItemView* view, const QString& fileName,
           bool innerFormat)
{
    QByteArray content;
    dataToByteArray(view, &content, csvSeparator_, innerFormat);

    QFile file(fileName);
    if (file.exists())
    {
        file.remove();
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    bool ret = false;
    if (!innerFormat)
    {
        // TODO localization.
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        QString unicodeOutput = codec->toUnicode(content);
        codec = QTextCodec::codecForName("Windows-1250");
        QByteArray encodedOutput = codec->fromUnicode(unicodeOutput);

        ret = (-1 != file.write(encodedOutput));
    }
    else
    {
        ret = (-1 != file.write(content));
    }

    return ret;
}

}  // namespace ExportUtilities
