#pragma once

#include <memory>

#include <QDialog>
#include <QMap>

class DatasetsListBrowser;
class DatasetDefinitionVisualization;
class ColumnsPreview;
class QDialogButtonBox;
class Dataset;

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

    std::unique_ptr<Dataset> getSelectedDataset();

    QString getZipFileName() const;

private:
    Ui::ImportData* ui;
};
