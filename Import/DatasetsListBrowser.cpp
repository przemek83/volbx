#include "DatasetsListBrowser.h"

#include <QMenu>
#include <QMessageBox>

#include "Common/DatasetUtilities.h"
#include "Datasets/DatasetInner.h"

#include "ui_DatasetsListBrowser.h"

DatasetsListBrowser::DatasetsListBrowser(QWidget* parent)
    : QWidget(parent), ui(new Ui::DatasetsListBrowser)
{
    ui->setupUi(this);

    connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
            &DatasetsListBrowser::searchTextChanged);

    ui->datasetsList->insertItems(
        0, DatasetUtilities::getListOfAvailableDatasets());

    ui->datasetsList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->datasetsList, &QListWidget::customContextMenuRequested, this,
            &DatasetsListBrowser::showContextMenu);

    connect(ui->datasetsList, &QListWidget::itemSelectionChanged, this,
            &DatasetsListBrowser::datasetsListItemSelectionChanged);
}

DatasetsListBrowser::~DatasetsListBrowser() { delete ui; }

QString DatasetsListBrowser::getSelectedDataset() const
{
    const QListWidgetItem* item{ui->datasetsList->currentItem()};
    if (item != nullptr)
        return item->text();

    return QString();
}

void DatasetsListBrowser::searchTextChanged(const QString& arg1)
{
    for (int i = 0; i < ui->datasetsList->count(); ++i)
    {
        QListWidgetItem* item{ui->datasetsList->item(i)};
        item->setHidden(!item->text().contains(arg1, Qt::CaseInsensitive));
    }
}

void DatasetsListBrowser::clearSelection()
{
    ui->datasetsList->clearSelection();
}

bool DatasetsListBrowser::isDatasetsListEmpty()
{
    return (ui->datasetsList->count() == 0);
}

void DatasetsListBrowser::showContextMenu(QPoint pos)
{
    // Create delete context menu.
    QPoint globalPos{ui->datasetsList->viewport()->mapToGlobal(pos)};

    if (ui->datasetsList->selectedItems().isEmpty() ||
        ui->datasetsList->itemAt(pos) == nullptr)
        return;

    const QIcon deleteIcon{
        QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton)};

    QMenu myMenu;
    myMenu.addAction(deleteIcon, QObject::tr("Delete dataset"));

    QAction* selectedItem{myMenu.exec(globalPos)};

    // Delete dataset.
    if (selectedItem != nullptr)
    {
        QString datasetName{ui->datasetsList->selectedItems().first()->text()};
        QMessageBox::StandardButton answer{QMessageBox::question(
            this, tr("Delete?"), tr("Delete dataset ") + datasetName + "?",
            QMessageBox::Yes | QMessageBox::No)};

        if (QMessageBox::Yes == answer)
        {
            if (!DatasetUtilities::removeDataset(datasetName))
                QMessageBox::warning(this, tr("Error"),
                                     tr("Can not delete ") + datasetName + ".");

            ui->datasetsList->clear();
            ui->datasetsList->insertItems(
                0, DatasetUtilities::getListOfAvailableDatasets());
        }
    }
}

void DatasetsListBrowser::datasetsListItemSelectionChanged()
{
    QString newCurrent{QLatin1String("")};
    QList<QListWidgetItem*> selectedItems{ui->datasetsList->selectedItems()};
    if (!selectedItems.isEmpty())
        newCurrent = selectedItems.at(0)->text();

    Q_EMIT currentDatasetChanged(newCurrent);
}
