#pragma once

#include <memory>

#include <QWidget>

class QTreeWidgetItem;
class Dataset;
class QComboBox;

namespace Ui
{
class DatasetVisualization;
}  // namespace Ui

/**
 * @brief Widget for visualization of dataset.
 */
class DatasetVisualization : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetVisualization(QWidget* parent = nullptr);

    ~DatasetVisualization() override;

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
    void setCurrentIndexUsingColumn(QComboBox* combo, unsigned int column);

    Ui::DatasetVisualization* ui;

    const QString typeNameString_{tr("Name")};
    const QString typeNameFloat_{tr("Number")};
    const QString typeNameDate_{tr("Date")};

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
