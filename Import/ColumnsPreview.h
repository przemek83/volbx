#pragma once

#include <memory>

#include <QTableWidget>

class Dataset;

/**
 * Columns preview widget. When set DatasetDefinition it displays
 * first 10 (or less if dataset has less than 10) rows of data.
 */
class ColumnsPreview : public QTableWidget
{
    Q_OBJECT
public:
    explicit ColumnsPreview(QWidget* parent = nullptr);

    ~ColumnsPreview() override = default;

    void setDatasetSampleInfo(const std::unique_ptr<Dataset>& dataset);

    void clear();

public Q_SLOTS:
    /**
     * Triggered when currently selected column in coupled widget changed.
     * Used to sync widgets displaying columns.
     * @param column currently selected column.
     */
    void selectCurrentColumn(int column);

private Q_SLOTS:
    /**
     * Triggered when selection in table changed.
     */
    void onItemSelectionChanged();

Q_SIGNALS:
    /**
     * Emit signal when selected column was changed to sync widgets
     * displaying columns.
     * @param currentColumn currently selected column.
     */
    void currentColumnNeedSync(int currentColumn);
};
