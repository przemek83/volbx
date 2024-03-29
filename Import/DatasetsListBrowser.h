#pragma once

#include <memory>

#include <QWidget>

#include "ui_DatasetsListBrowser.h"

class QListWidgetItem;

/**
 * @brief Widget for browsing list of actual datasets.
 */
class DatasetsListBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetsListBrowser(QWidget* parent = nullptr);

    void clearSelection();

    bool isDatasetsListEmpty() const;

private:
    void setupDatasetsList();

    bool doesUserChooseToDeleteSelectedDataset(QPoint pos);

    bool doesUserConfirmedDeleting(const QString& datasetToDelete);

    void deleteSelectedDataset(const QString& datasetToDelete);

    std::unique_ptr<Ui::DatasetsListBrowser> ui_;

private Q_SLOTS:
    void searchTextChanged(const QString& arg1);

    void showContextMenu(QPoint pos);

    void datasetsListItemSelectionChanged();

Q_SIGNALS:
    void currentDatasetChanged(QString current);
};
