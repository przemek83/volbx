#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include <memory>

#include <QDialog>
#include <QMap>

class DatasetsListBrowser;
class DatasetDefinitionVisualization;
class ColumnsPreview;
class DatasetDefinitionInner;
class DatasetDefinition;
class QDialogButtonBox;

namespace Ui
{
class ImportData;
}  // namespace Ui

/**
 * @brief Main dialog for loading data into application.
 * It consist QTabWidget with pages for each type of data.
 */
class ImportData : public QDialog
{
    Q_OBJECT
public:
    explicit ImportData(QWidget* parent = nullptr);

    ~ImportData() override;

    ImportData& operator=(const ImportData& other) = delete;
    ImportData(const ImportData& other) = delete;

    ImportData& operator=(ImportData&& other) = delete;
    ImportData(ImportData&& other) = delete;

    std::unique_ptr<DatasetDefinition> getSelectedDataset();

    /// Type enum used during creation of Dataset objects.
    enum ImportDataType
    {
        IMPORT_TYPE_INNER = 0,
        IMPORT_TYPE_SPREADSHEET
    };

    ImportDataType getImportDataType() const;

    QString getZipFileName() const;

private:
    Ui::ImportData* ui;
};

#endif  // IMPORTDATA_H
