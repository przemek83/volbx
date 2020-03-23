#include "ExportData.h"

#include <ProgressBarCounter.h>
#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QItemSelectionModel>
#include <QMimeData>
#include <QtXml/QDomDocument>
#include <quazip5/quazip.h>
#include <QwtBleUtilities.h>

#include "Datasets/Dataset.h"
#include "Datasets/DatasetDefinition.h"
#include "Datasets/DatasetDefinitionInner.h"
#include "Datasets/DatasetInner.h"
#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"
#include "Shared/Logger.h"

#include "Constants.h"

namespace
{
constexpr char csvSeparator_ {';'};

constexpr char tsvSeparator_ {'\t'};

const QString& getCellTypeTag(QVariant& cell)
{
    switch (cell.type())
    {
        case QVariant::Date:
        case QVariant::DateTime:
        {
            static const QString dateTag(QStringLiteral("s=\"3\""));
            cell = QVariant(-1 * cell.toDate().daysTo(Constants::getStartOfExcelWorld()));
            return dateTag;
        }

        case QVariant::Int:
        case QVariant::Double:
        {
            static const QString numericTag(QStringLiteral("s=\"4\""));
            return numericTag;
        }

        default:
        {
            static const QString stringTag{QStringLiteral("t=\"str\"")};
            return stringTag;
        }
    }
}

QByteArray gatherSheetContent(const QAbstractItemView* view)
{
    QByteArray rowsContent;
    rowsContent.append("<sheetData>");

    QStringList columnNames = Constants::generateExcelColumnNames(Constants::MAX_EXCEL_COLUMNS);
    auto proxyModel = qobject_cast<FilteringProxyModel*>(view->model());

    Q_ASSERT(nullptr != proxyModel);

    bool multiSelection =
        (QAbstractItemView::MultiSelection == view->selectionMode());
    QItemSelectionModel* selectionModel = view->selectionModel();

    int proxyColumnCount = proxyModel->columnCount();
    int proxyRowCount = proxyModel->rowCount();

    //Add headers using string type.
    rowsContent.append(R"(<row r="1" spans="1:1" x14ac:dyDescent="0.25">)");
    for (int j = 0; j < proxyColumnCount; ++j)
    {
        QString header = proxyModel->headerData(j, Qt::Horizontal).toString();
        QString clearedHeader(header.replace(QRegExp(QStringLiteral("[<>&\"']")), QStringLiteral(" ")).replace(QStringLiteral("\r\n"), QStringLiteral(" ")));
        rowsContent.append("<c r=\"" + columnNames.at(j));
        rowsContent.append(R"(1" t="str" s="6"><v>)" + clearedHeader + "</v></c>");
    }
    rowsContent.append("</row>");

    int skippedRows = 0;

    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::SAVING);
    ProgressBarCounter bar(barTitle, proxyModel->rowCount(), nullptr);
    bar.showDetached();

    //For each row.
    for (int i = 0; i < proxyRowCount; ++i)
    {
        if (multiSelection &&
            !selectionModel->isSelected(proxyModel->index(i, 0)))
        {
            skippedRows++;
            continue;
        }

        //Create new row.
        QString rowNumber(QString::fromLatin1(QByteArray::number(i - skippedRows + 2)));
        rowsContent.append("<row r=\"" + rowNumber);
        rowsContent.append(R"(" spans="1:1" x14ac:dyDescent="0.25">)");

        //For each column.
        for (int j = 0; j < proxyColumnCount; ++j)
        {
            QVariant cell = proxyModel->index(i, j).data();
            if (cell.isNull())
            {
                continue;
            }

            //Create cell.
            rowsContent.append("<c r=\"" + columnNames.at(j) + rowNumber + "\" ");
            rowsContent.append(getCellTypeTag(cell));
            rowsContent.append("><v>" + cell.toByteArray() + "</v></c>");
        }
        rowsContent.append("</row>");

        bar.updateProgress(i + 1);
    }

    //Close xml tag for data.
    rowsContent.append("</sheetData>");

    return rowsContent;
}

void variantToString(const QVariant& variant,
                     QByteArray* destinationArray,
                     char separator,
                     bool innerFormat)
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
                QString value(QwtBleUtilities::doubleToStringUsingLocale(variant.toDouble(), 2));
                destinationArray->append(value);
            }
            break;
        }

        case QVariant::Date:
        case QVariant::DateTime:
        {
            static const QString defDateFormat(QwtBleUtilities::getDefaultDateFormat());
            destinationArray->append(variant.toDate().toString(defDateFormat));
            break;
        }

        case QVariant::String:
        {
            QString tmpString(variant.toString());
            if (separator == tsvSeparator_)
            {
                //Simplification -> change tabs and new lines into spaces.
                tmpString =
                    tmpString.replace(QRegExp("[" + QString(tsvSeparator_) + "\n" + "]"),
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
                     QByteArray* destinationArray,
                     char separator,
                     bool innerFormat)
{
    /*
     * Using standards described on:
     * http://en.wikipedia.org/wiki/Comma-separated_values
     * and
     * http://tools.ietf.org/html/rfc4180
     */

    auto proxyModel = qobject_cast<FilteringProxyModel*>(view->model());
    Q_ASSERT(nullptr != proxyModel);

    int proxyColumnCount = proxyModel->columnCount();

    //Save column names.
    if (!innerFormat)
    {
        for (int j = 0; j < proxyColumnCount; ++j)
        {
            destinationArray->append(proxyModel->headerData(j, Qt::Horizontal).toString());
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
                variantToString(actualField,
                                destinationArray,
                                separator,
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

std::tuple<bool, int, QByteArray> saveDatasetDataFile(QuaZipFile& zipFile,
                                                      const QAbstractItemView* view,
                                                      FilteringProxyModel* proxyModel,
                                                      ProgressBarCounter& bar)
{
    bool result = zipFile.open(QIODevice::WriteOnly,
                               QuaZipNewInfo(Constants::getDatasetDataFilename()));
    if (!result)
    {
        LOG(LogTypes::IMPORT_EXPORT, "Error while saving data file.");
        return {false, 0, {}};
    }

    bool multiSelection = (QAbstractItemView::MultiSelection == view->selectionMode());
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
        if (multiSelection && !selectionModel->isSelected(proxyModel->index(i, 0)))
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
                        zipFile.write(QByteArray::number(actualField.toDate().toJulianDay()));
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
                            //No new line for first string.
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
    bool result = zipFile.open(QIODevice::WriteOnly,
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
                               const QAbstractItemView* view,
                               int rowCount)
{
    const TableModel* parentModel =
        (qobject_cast<FilteringProxyModel*>(view->model()))->getParentModel();

    QByteArray definitionContent;
    parentModel->getDatasetDefinition()->toXml(definitionContent, rowCount);

    bool result = zipFile.open(QIODevice::WriteOnly,
                               QuaZipNewInfo(Constants::getDatasetDefinitionFilename()));
    if (!result || zipFile.write(definitionContent) == -1)
    {
        LOG(LogTypes::IMPORT_EXPORT, "Error while saving definition file.");
        return false;
    }
    zipFile.close();

    return true;
}
}; // namespace

namespace ExportData
{

bool asXLSX(const QAbstractItemView* view, const QString& fileName)
{
    Q_ASSERT(nullptr != view);

    // QuaZip has problems with opening file from resources. Workaround it...
    QFile::copy(QStringLiteral(":/template.xlsx"), QStringLiteral("template.xlsx"));

    //Open xlsx template.
    QuaZip inZip(QStringLiteral("template.xlsx"));
    inZip.open(QuaZip::mdUnzip);

    //Files list in template.
    QStringList fileList = inZip.getFileNameList();

    //Create out xlsx.
    QuaZip outZip(fileName);
    outZip.open(QuaZip::mdCreate);

    //For each file in template.
    for (const QString& file : fileList)
    {
        inZip.setCurrentFile(file);

        QuaZipFile inZipFile(&inZip);
        if (!inZipFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            LOG(LogTypes::IMPORT_EXPORT,
                "Can not open file " + inZipFile.getFileName() + ".");
            return false;
        }

        QuaZipFile outZipFile(&outZip);
        if (!outZipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(file)))
        {
            LOG(LogTypes::IMPORT_EXPORT, "Can not open file " + inZipFile.getFileName() + ".");
            return false;
        }

        //Modify sheet1 by inserting data from view, copy rest of files.
        if (file.endsWith(QLatin1String("sheet1.xml")))
        {
            const QByteArray rowsContent = gatherSheetContent(view);

            //Replace empty tag by gathered data.
            QByteArray content = inZipFile.readAll();
            content.replace("<sheetData/>", rowsContent);

            outZipFile.write(content);
        }
        else
        {
            outZipFile.write(inZipFile.readAll());
        }

        inZipFile.close();
        outZipFile.close();
    }

    outZip.close();

    return true;
}

bool saveDataset(const QString& name,
                 const QAbstractItemView* view)
{
    Q_ASSERT(nullptr != view);

    QTime performanceTimer;
    performanceTimer.start();

    LOG(LogTypes::IMPORT_EXPORT, "Saving dataset " + name);

    //Open archive.
    QuaZip zip(DatasetInner::getDatasetsDir() + name + Constants::getDatasetExtension());
    bool openSuccess = zip.open(QuaZip::mdCreate);
    if (!openSuccess)
    {
        return false;
    }

    //Data file, write directly in loop.
    //Only one zip file in archive can be accessed at a time.
    QuaZipFile zipFile(&zip);
    auto proxyModel = qobject_cast<FilteringProxyModel*>(view->model());
    Q_ASSERT(nullptr != proxyModel);

    QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::SAVING);
    ProgressBarCounter bar(barTitle, proxyModel->rowCount(), nullptr);
    bar.showDetached();

    auto [success, rowCount, stringsContent] = saveDatasetDataFile(zipFile,
                                                                   view,
                                                                   proxyModel,
                                                                   bar);

    if (!success)
    {
        return false;
    }

    //Save strings.
    success = saveDatasetStringsFile(zipFile, stringsContent);
    if (!success)
    {
        return false;
    }

    //Save definition.
    success = saveDatasetDefinitionFile(zipFile, view, rowCount);
    if (!success)
    {
        return false;
    }

    LOG(LogTypes::IMPORT_EXPORT, "Saved dataset having " +
        QString::number(rowCount) +
        " rows in time " +
        QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
        " seconds.");

    return success;
}

void quickAsTSV(const QAbstractItemView* view)
{
    QByteArray content;
    dataToByteArray(view, &content, tsvSeparator_, false);

    QApplication::clipboard()->setText(QString::fromUtf8(content));
}


bool asCsv(const QAbstractItemView* view,
           const QString& fileName,
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

} // namespace ExportData

