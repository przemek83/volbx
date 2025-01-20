#pragma once

#include <memory>

#include <QWidget>

#include "ui_DatasetsListBrowser.h"

class QListWidgetItem;

class DatasetsListBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetsListBrowser(QWidget* parent);

    void clearSelection();

    bool isDatasetsListEmpty() const;

private:
    void setupDatasetsList();

    bool userWantToDeleteDataset(QPoint pos) const;

    bool userConfirmedDeleting(const QString& datasetToDelete);

    void deleteSelectedDataset(const QString& datasetToDelete);

    std::unique_ptr<Ui::DatasetsListBrowser> ui_{
        std::make_unique<Ui::DatasetsListBrowser>()};

private slots:
    void searchTextChanged(const QString& arg1) const;

    void showContextMenu(QPoint pos);

    void datasetListItemSelectionChanged();

signals:
    void currentDatasetChanged(QString current);
};
