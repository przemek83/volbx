#ifndef DATASETSLISTBROWSER_H
#define DATASETSLISTBROWSER_H

#include <QWidget>

class QListWidgetItem;

namespace Ui {
    class DatasetsListBrowser;
}

/**
 * @brief Widget for browsing actual datasets list.
 */
class DatasetsListBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetsListBrowser(QWidget* parent = nullptr);

    ~DatasetsListBrowser() override;

    QString getSelectedDataset() const;

    void clearSelection();

    bool isDatasetsListEmpty();

private:
    Q_DISABLE_COPY(DatasetsListBrowser)

    Ui::DatasetsListBrowser *ui;

private slots:
    void searchTextChanged(const QString& arg1);

    void showContextMenu(const QPoint& pos);

    void on_datasetsList_itemSelectionChanged();

signals:
    void currentDatasetChanged(QString current);
};

#endif // DATASETSLISTBROWSER_H
