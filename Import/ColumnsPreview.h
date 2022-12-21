#pragma once

#include <memory>

#include <QTableWidget>

class Dataset;

/**
 * @brief Columns preview widget.
 * When set DatasetDefinition it displays first 10 (or less if dataset has less
 * than 10) rows of data.
 */
class ColumnsPreview : public QTableWidget
{
    Q_OBJECT
public:
    explicit ColumnsPreview(QWidget* parent = nullptr);

    void setDatasetSampleInfo(const std::unique_ptr<Dataset>& dataset);

    void clear();

public Q_SLOTS:
    /**
     * Triggered when currently selected column in related widget changed.
     * Used to sync widgets displaying columns.
     * @param column Currently selected column.
     */
    void selectCurrentColumn(int column);

private:
    static QTableWidgetItem* createItem(const QString& name);

    void setLabels(const std::unique_ptr<Dataset>& dataset);

private Q_SLOTS:
    /**
     * Triggered when selection in table changed.
     */
    void onItemSelectionChanged();

Q_SIGNALS:
    /**
     * Emit when selected column was changed to sync widgets displaying columns.
     * @param currentColumn Currently selected column.
     */
    void currentColumnNeedSync(int currentColumn);
};
