#pragma once

#include <QWidget>

class QListWidgetItem;

namespace Ui
{
class DatasetsListBrowser;
}  // namespace Ui

/**
 * @brief Widget for browsing actual datasets list.
 */
class DatasetsListBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetsListBrowser(QWidget* parent = nullptr);

    ~DatasetsListBrowser() override;

    void clearSelection();

    bool isDatasetsListEmpty();

private:
    void setupDatasetsList();

    Ui::DatasetsListBrowser* ui;

private Q_SLOTS:
    void searchTextChanged(const QString& arg1);

    void showContextMenu(QPoint pos);

    void datasetsListItemSelectionChanged();

Q_SIGNALS:
    void currentDatasetChanged(QString current);
};
