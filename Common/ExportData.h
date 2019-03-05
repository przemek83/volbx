#ifndef EXPORTDATA_H
#define EXPORTDATA_H

#include <QString>
#include <QVariant>
#include <vector>
#include <quazip5/quazipfile.h>

#include "Common/Formats.h"
#include "Common/SpecialColumns.h"

class DatasetDefinition;
class QAbstractItemView;
class FilteringProxyModel;
class ProgressBar;
class QItemSelectionModel;

/**
 *  @brief class with static methods used for export.
 */
class ExportData
{
public:
    ExportData() = delete;
    ~ExportData() = delete;

    ExportData& operator=(const ExportData& other) = delete;
    ExportData(const ExportData& other) = delete;

    ExportData& operator=(ExportData&& other) = delete;
    ExportData(ExportData&& other) = delete;

    static bool exportAsXLSX(const QAbstractItemView* view, QString fileName);

    static void quickExportAsTSV(const QAbstractItemView* view);

    static bool exportAsCsv(const QAbstractItemView* view,
                            QString fileName,
                            bool innerFormat);

    static bool saveDataset(QString name, const QAbstractItemView* view);

private:
    static void dataToByteArray(const QAbstractItemView* view,
                                QByteArray* destinationArray,
                                QString separator,
                                bool innerFormat);

    static void variantToString(const QVariant& variant,
                                QByteArray* destinationArray,
                                QString separator,
                                bool innerFormat);

    static const char* csvSeparator_;

    static const char* tsvSeparator_;

    static std::tuple<bool, int> saveDatasetDataFile(QuaZipFile& zipFile,
                                                     const QAbstractItemView* view,
                                                     FilteringProxyModel* proxyModel,
                                                     QByteArray& stringsContent,
                                                     ProgressBar* bar);

    static bool saveDatasetStringsFile(QuaZipFile& zipFile,
                                       QByteArray& stringsContent);

    static bool saveDatasetDefinitionFile(QuaZipFile& zipFile,
                                          const QAbstractItemView* view,
                                          int rowCount);

    static void gatherSheetContent(QByteArray& rowsContent,
                                   const QAbstractItemView* view);

    static const QString& getCellTypeTag(QVariant& cell);
};

#endif // EXPORTDATA_H
