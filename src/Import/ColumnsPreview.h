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
    explicit ColumnsPreview(QWidget* parent = nullptr);

    void setDatasetSampleInfo(const std::unique_ptr<Dataset>& dataset);

    void clear();

public Q_SLOTS:
    void selectCurrentColumn(int column);

private:
    static QTableWidgetItem* createItem(const QString& name);

    void setLabels(const std::unique_ptr<Dataset>& dataset);

private Q_SLOTS:
    void onItemSelectionChanged();

Q_SIGNALS:
    void currentColumnNeedSync(int currentColumn);
};
