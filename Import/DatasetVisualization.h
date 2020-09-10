#pragma once

#include <memory>

#include <QWidget>

#include <ColumnTag.h>

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
     * Triggered when currently selected column in linked widget changed.
     * Used to sync widgets displaying columns.
     * @param column Currently selected column.
     */
    void selectCurrentColumn(int column);

private:
    static void setCurrentIndexUsingColumn(QComboBox* combo, int column);

    void setupColumnsListWidget();

    void setTaggedColumns();

    QVector<bool> getActiveColumns() const;

    void setTaggedColumnInDataset(ColumnTag tag, QComboBox* combo);

    QString getTypeDisplayNameForGivenColumn(int column) const;

    void fillTaggedColumnCombos();

    void setAllItemsInColumnsListToState(Qt::CheckState state);

    static int getCurrentValueFromCombo(QComboBox* combo);

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

    void refreshColumnList(int newIndex);

Q_SIGNALS:
    /**
     * Emit when selected column was changed to sync linked widgets.
     * @param currentColumn Currently selected column.
     */
    void currentColumnNeedSync(int currentColumn);
};
