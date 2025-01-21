#pragma once

#include <memory>

#include <QTableWidget>

class Dataset;

/// @brief Columns preview widget.
/// Displays max 10 first rows of data.
class ColumnsPreview : public QTableWidget
{
    Q_OBJECT
public:
    explicit ColumnsPreview(QWidget* parent);

    void setDatasetSampleInfo(const std::unique_ptr<Dataset>& dataset);

    void purge();

public slots:
    void selectCurrentColumn(int column);

private:
    static QTableWidgetItem* createItem(const QString& name);

    void setLabels(const std::unique_ptr<Dataset>& dataset);

private slots:
    void onItemSelectionChanged();

signals:
    void currentColumnNeedSync(int currentColumn);
};
