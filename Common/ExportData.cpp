 #include <QDebug>
#include <QDate>
#include <QClipboard>
#include <QApplication>
#include <QFile>
#include <QMimeData>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <quazip5/quazip.h>

#include "Constants.h"
#include "Shared/Logger.h"
#include "Datasets/DatasetDefinition.h"
#include "Datasets/Dataset.h"
#include "Datasets/DatasetInner.h"
#include "Datasets/DatasetDefinitionInner.h"
#include "ProgressBar.h"
#include "ModelsAndViews/TableModel.h"
#include "ModelsAndViews/FilteringProxyModel.h"

#include "ExportData.h"

const char* ExportData::csvSeparator_ = ";";

const char* ExportData::tsvSeparator_ = "\t";

ExportData::ExportData()
{
    Q_ASSERT(false);
}

ExportData::~ExportData()
{
    Q_ASSERT(false);
}

void ExportData::gatherSheetContent(QByteArray& rowsContent,
                                    const QAbstractItemView* view)
{
    rowsContent.append("<sheetData>");

    QStringList columnNames;
    Constants::generateExcelColumnNames(columnNames, 600);

    FilteringProxyModel* proxyModel =
        static_cast<FilteringProxyModel*>(view->model());

    Q_ASSERT(NULL != proxyModel);

    bool multiSelection =
        (QAbstractItemView::MultiSelection == view->selectionMode());
    QItemSelectionModel* selectionModel = view->selectionModel();

    int proxyColumnCount = proxyModel->columnCount();
    int proxyRowCount = proxyModel->rowCount();

    //Add headers using string type.
    rowsContent.append("<row r=\"1\" spans=\"1:1\" x14ac:dyDescent=\"0.25\">");
    for(int j = 0; j<proxyColumnCount; ++j)
    {
        QString header = proxyModel->headerData(j, Qt::Horizontal).toString();
        QString clearedHeader(header.replace(QRegExp("[<>&\"']")," ").replace("\r\n"," "));
        rowsContent.append("<c r=\"" + columnNames.at(j));
        rowsContent.append("1\" t=\"str\" s=\"6\"><v>" + clearedHeader + "</v></c>");
    }
    rowsContent.append("</row>");

    int skippedRows = 0;

    ProgressBar bar(ProgressBar::PROGRESS_TITLE_SAVING,
                    proxyModel->rowCount(), NULL);

    //For each row.
    for(int i = 0; i < proxyRowCount; ++i)
    {
        if( true == multiSelection &&
            false == selectionModel->isSelected(proxyModel->index(i, 0)) )
        {
            skippedRows++;
            continue;
        }

        //Create new row.
        QString rowNumber(QByteArray::number(i-skippedRows+2));
        rowsContent.append("<row r=\"" + rowNumber);
        rowsContent.append("\" spans=\"1:1\" x14ac:dyDescent=\"0.25\">");

        //For each column.
        for(int j = 0; j < proxyColumnCount; ++j)
        {
            QVariant cell = proxyModel->index(i, j).data();
            if ( true == cell.isNull() )
            {
                continue;
            }

            //Create cell.
            rowsContent.append("<c r=\""+ columnNames.at(j) + rowNumber + "\" ");
            rowsContent.append(getCellTypeTag(cell));
            rowsContent.append("><v>"+cell.toByteArray()+"</v></c>");
        }
        rowsContent.append("</row>");

        bar.updateProgress(i+1);
    }

    //Close xml tag for data.
    rowsContent.append("</sheetData>");
}

bool ExportData::exportAsXLSX(const QAbstractItemView* view, QString fileName)
{
    Q_ASSERT(NULL != view);

    //Open xlsx template.
    QuaZip inZip(":/template.xlsx");
    inZip.open(QuaZip::mdUnzip);

    //Files list in template.
    QStringList fileList = inZip.getFileNameList();

    //Create out xlsx.
    QuaZip outZip(fileName);
    outZip.open(QuaZip::mdCreate);

    //For each file in template.
    foreach(QString file, fileList)
    {
        inZip.setCurrentFile(file);

        QuaZipFile inZipFile(&inZip);
        if ( false == inZipFile.open(QIODevice::ReadOnly | QIODevice::Text) )
        {
            LOG(LOG_IMPORT_EXPORT,
                "Can not open file " + inZipFile.getFileName() + ".");
            return false;
        }

        QuaZipFile outZipFile(&outZip);
        if ( false == outZipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(file)) )
        {
            LOG(LOG_IMPORT_EXPORT,
                "Can not open file " + inZipFile.getFileName() + ".");
            return false;
        }

        //Modify sheet1 by inserting data from view, copy rest of files.
        if( file.endsWith("sheet1.xml") )
        {
            QByteArray rowsContent;
            gatherSheetContent(rowsContent, view);

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

const QString& ExportData::getCellTypeTag(QVariant& cell)
{
    switch(cell.type())
    {
        case QVariant::Date:
        case QVariant::DateTime:
        {
            const static QString dateTag("s=\"3\"");
            const static QDate startOfTheExcelWorld(1899, 12, 30);
            cell = QVariant(-1*cell.toDate().daysTo(startOfTheExcelWorld));
            return dateTag;
            break;
        }

        case QVariant::Int:
        case QVariant::Double:
        {
            const static QString numericTag("s=\"4\"");
            return numericTag;
            break;
        }

        default:
        {
            const static QString stringTag("t=\"str\"");
            return stringTag;
            break;
        }
    }
}

void ExportData::quickExportAsTSV(const QAbstractItemView* view)
{
    QByteArray content;
    dataToByteArray(view, &content, tsvSeparator_, false);

    QApplication::clipboard()->setText(content);
}

void ExportData::dataToByteArray(const QAbstractItemView* view,
                                 QByteArray* destinationArray,
                                 QString separator,
                                 bool innerFormat)
{
    /*
     * Using standards described on:
     * http://en.wikipedia.org/wiki/Comma-separated_values
     * and
     * http://tools.ietf.org/html/rfc4180
     */

    FilteringProxyModel* proxyModel =
        static_cast<FilteringProxyModel*>(view->model());
    Q_ASSERT(NULL != proxyModel);

    int proxyColumnCount = proxyModel->columnCount();

    //Save column names.
    if(false == innerFormat)
    {
        for(int j = 0; j < proxyColumnCount; ++j)
        {
            destinationArray->append(proxyModel->headerData(j, Qt::Horizontal).toString());
            if(j != proxyColumnCount-1)
                destinationArray->append(separator);
        }
        destinationArray->append("\n");
    }

    ProgressBar bar(ProgressBar::PROGRESS_TITLE_SAVING,
                    proxyModel->rowCount(),
                    NULL);

    bool multiSelection =
        (QAbstractItemView::MultiSelection == view->selectionMode());
    QItemSelectionModel* selectionModel = view->selectionModel();

    int proxyRowCount = proxyModel->rowCount();
    for(int i = 0; i < proxyRowCount; ++i)
    {
        if( true == multiSelection &&
            false == selectionModel->isSelected(proxyModel->index(i, 0)) )
        {
            continue;
        }

        for(int j = 0; j < proxyColumnCount; ++j)
        {
            QVariant actualField = proxyModel->index(i, j).data();

            if ( false == actualField.isNull() )
            {
                variantToString(actualField,
                                destinationArray,
                                separator,
                                innerFormat);
            }

            if(j != proxyColumnCount-1)
                destinationArray->append(separator);

        }
        destinationArray->append("\n");
        bar.updateProgress(i+1);
    }
}

void ExportData::variantToString(QVariant& variant,
                                 QByteArray* destinationArray,
                                 QString separator,
                                 bool innerFormat)
{
    switch(variant.type())
    {
        case QVariant::Double:
        case QVariant::Int:
        {
            if(true == innerFormat)
            {
                destinationArray->append(variant.toString());
            }
            else
            {
                QString value(Constants::floatToStringUsingLocale(variant.toDouble(), 2));
                destinationArray->append(value);
            }
            break;
        }

        case QVariant::Date:
        case QVariant::DateTime:
        {
            static const QString defDateFormat(Constants::defaultDateFormat_);
            destinationArray->append(variant.toDate().toString(defDateFormat));
            break;
        }

        case QVariant::String:
        {
            QString tmpString(variant.toString());
            if(separator == tsvSeparator_)
            {
                //Simplification -> change tabs and new lines into spaces.
                tmpString =
                    tmpString.replace(QRegExp("[" + QString(tsvSeparator_) + "\n" + "]"),
                                      " ");
                destinationArray->append(tmpString);
            }
            else
            {
                tmpString.replace('"', "\"\"");
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

bool ExportData::exportAsCsv(const QAbstractItemView* view,
                             QString fileName,
                             bool innerFormat)
{
    QByteArray content;
    dataToByteArray(view, &content, csvSeparator_, innerFormat);

    QFile file(fileName);
    if(file.exists())
        file.remove();

    if (!file.open(QIODevice::WriteOnly))
        return false;

    bool ret = false;
    if(false == innerFormat)
    {
        //TODO localization.
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
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

bool ExportData::saveDataset(QString name,
                             const QAbstractItemView* view)
{
    Q_ASSERT(NULL != view);

    QTime performanceTimer;
    performanceTimer.start();

    LOG(LOG_IMPORT_EXPORT, "Saving dataset " + name);

    //Open archive.
    QuaZip zip(DatasetInner::getDatasetsDir() + name + Constants::datasetExtension_);
    bool result = zip.open(QuaZip::mdCreate);
    if( false == result)
    {
        return result;
    }

    //Data file, write directly in loop.
    //Only one zip file in archive can be accessed at a time.
    QuaZipFile zipFile(&zip);

    FilteringProxyModel* proxyModel =
        static_cast<FilteringProxyModel*>(view->model());
    Q_ASSERT(NULL != proxyModel);
    ProgressBar bar(ProgressBar::PROGRESS_TITLE_SAVING,
                    proxyModel->rowCount(),
                    NULL);

    //Save data.
    int rowCount = 0;
    QByteArray stringsContent;
    result = saveDatasetDataFile(zipFile,
                                 view,
                                 rowCount,
                                 proxyModel,
                                 stringsContent,
                                 &bar);
    if( false == result )
    {
        return false;
    }

    //Save strings.
    result = saveDatasetStringsFile(zipFile, stringsContent);
    if( false == result )
    {
        return false;
    }

    //Save definition.
    result = saveDatasetDefinitionFile(zipFile, view, rowCount);
    if( false == result )
    {
        return false;
    }

    LOG(LOG_IMPORT_EXPORT, "Saved dataset having " +
                           QString::number(rowCount) +
                           " rows in time " +
                           QString::number(performanceTimer.elapsed()*1.0/1000) +
                           " seconds.");

    return result;
}

bool ExportData::saveDatasetDataFile(QuaZipFile& zipFile,
                                     const QAbstractItemView* view,
                                     int& rowCount,
                                     FilteringProxyModel* proxyModel,
                                     QByteArray& stringsContent,
                                     ProgressBar* bar)
{
    bool result = zipFile.open(QIODevice::WriteOnly,
                               QuaZipNewInfo(Constants::datasetDataFilename_));
    if( false == result)
    {
        LOG(LOG_IMPORT_EXPORT, "Error while saving data file.");
        return false;
    }

    bool multiSelection = (QAbstractItemView::MultiSelection == view->selectionMode());
    QItemSelectionModel* selectionModel = view->selectionModel();

    int proxyRowCount = proxyModel->rowCount();
    int proxyColumnCount = proxyModel->columnCount();
    QHash<QString, int> stringsMap;
    int nextIndex = 1;
    static const char* newLine = "\n";
    for( int i = 0; i < proxyRowCount; ++i )
    {
        if( true == multiSelection &&
            false == selectionModel->isSelected(proxyModel->index(i, 0)) )
            continue;

        for( int j = 0; j < proxyColumnCount; ++j )
        {
            const QVariant& actualField = proxyModel->index(i, j).data();

            if( false == actualField.isNull() )
            {
                switch( actualField.type() )
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
                        if( 0 == index )
                        {
                            index = nextIndex;
                            tmpString.replace(newLine, "\t");
                            //No new line for first string.
                            if( 1 != nextIndex )
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

            if( j != proxyColumnCount - 1 )
                zipFile.write(csvSeparator_);
        }

        zipFile.write(newLine);
        rowCount++;

        bar->updateProgress(i+1);
    }

    zipFile.close();

    return true;
}

bool ExportData::saveDatasetStringsFile(QuaZipFile& zipFile,
                                        QByteArray& stringsContent)
{
    //Save strings file.
    bool result = zipFile.open(QIODevice::WriteOnly,
                               QuaZipNewInfo(Constants::datasetStringsFilename_));
    if( false == result || -1 == zipFile.write(stringsContent) )
    {
        LOG(LOG_IMPORT_EXPORT, "Error while saving strings file.");
        return false;
    }
    zipFile.close();

    return true;
}

bool ExportData::saveDatasetDefinitionFile(QuaZipFile& zipFile,
                                           const QAbstractItemView* view,
                                           int rowCount)
{
    //Save definition file.
    const TableModel* parentModel =
        (static_cast<FilteringProxyModel*>(view->model()))->getParentModel();
    const DatasetDefinitionInner* definition =
        static_cast<const DatasetDefinitionInner*>(parentModel->getDatasetDefinition());
    QByteArray definitionContent;
    definition->toXml(definitionContent, rowCount);

    bool result = zipFile.open(QIODevice::WriteOnly,
                               QuaZipNewInfo(Constants::datasetDefinitionFilename_));
    if( false == result || -1 == zipFile.write(definitionContent) )
    {
        LOG(LOG_IMPORT_EXPORT, "Error while saving definition file.");
        return false;
    }
    zipFile.close();

    return true;
}
