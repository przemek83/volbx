#ifndef DATASETDEFINITIONVISUALIZATION_H
#define DATASETDEFINITIONVISUALIZATION_H

#include <memory>

#include <QWidget>

class QTreeWidgetItem;
class Dataset;

namespace Ui
{
class DatasetDefinitionVisualization;
}  // namespace Ui

/**
 * @brief Widget for visualization of dataset definition.
 */
class DatasetDefinitionVisualization : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetDefinitionVisualization(QWidget* parent = nullptr);

    ~DatasetDefinitionVisualization() override;

    void setDataset(std::unique_ptr<Dataset> dataset);

    void clear();

    std::unique_ptr<Dataset> retrieveDataset();

public Q_SLOTS:
    /**
     * Triggered when currently selected column in coupled widget changed.
     * Used to sync widgets displaying columns.
     * @param column currently selected column.
     */
    void selectCurrentColumn(int column);

private:
    Ui::DatasetDefinitionVisualization* ui;

    const QString typeNameString_;

    const QString typeNameFloat_;

    const QString typeNameDate_;

    std::unique_ptr<Dataset> dataset_{nullptr};

private Q_SLOTS:
    void searchTextChanged(const QString& newText);

    void currentColumnOnTreeChanged(QTreeWidgetItem* current,
                                    QTreeWidgetItem* previous);

    void selectAllClicked();

    void unselectAllClicked();

    void taggedColumnChanged(int newIndex);

Q_SIGNALS:
    /**
     * Emit signal when selected column was changed to sync widgets
     * displaying columns.
     * @param currentColumn currently selected column.
     */
    void currentColumnNeedSync(int currentColumn);
};

#endif  // DATASETDEFINITIONVISUALIZATION_H
