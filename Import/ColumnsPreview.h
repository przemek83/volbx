#ifndef COLUMNSPREVIEW_H
#define COLUMNSPREVIEW_H

#include <QTableWidget>

class DatasetDefinition;

/**
 * Columns preview widget. When set DatasetDefinition it displays
 * first 10 (or less if dataset has less than 10) rows of data.
 */
class ColumnsPreview : public QTableWidget
{
    Q_OBJECT
public:
    explicit ColumnsPreview(QWidget* parent = nullptr);

    virtual ~ColumnsPreview() = default;

    void setDatasetDefinitionSampleInfo(const DatasetDefinition* datasetDefinition);

    void clearDataAndDisable();

public slots:
    /**
     * Trigerred when currently selected column in coupled widget changed.
     * Used to sync widgets displaying columns.
     * @param column currently selected column.
     */
    void selectCurrentColumn(int column);

private:
    Q_DISABLE_COPY(ColumnsPreview)

private slots:
    /**
     * Trigerred when selection in table changed.
     */
    void selectionChanged();

signals:
    /**
     * Emit signal when selected column was changed to sync widgets
     * displaying columns.
     * @param currentColumn currently selected column.
     */
    void currentColumnNeedSync(int currentColumn);
};

#endif // COLUMNSPREVIEW_H
