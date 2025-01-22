#pragma once

#include <QTableWidget>

class Dataset;

/// @brief Columns preview widget.
/// Displays max 10 first rows of data.
class ColumnsPreview : public QTableWidget
{
    Q_OBJECT
public:
    explicit ColumnsPreview(QWidget* parent);

    void setDatasetSampleInfo(int columnsCount,
                              const QVector<QVector<QVariant>>& sampleData);

    void purge();

public slots:
    void selectCurrentColumn(int column);

private:
    static QTableWidgetItem* createItem(const QString& name);

private slots:
    void onItemSelectionChanged();

signals:
    void currentColumnNeedSync(int currentColumn);
};
