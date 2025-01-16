#include "DatasetsListBrowser.h"

#include <QMenu>
#include <QMessageBox>

#include <Common/DatasetUtilities.h>

DatasetsListBrowser::DatasetsListBrowser(QWidget* parent)
    : QWidget(parent), ui_(std::make_unique<Ui::DatasetsListBrowser>())
{
    ui_->setupUi(this);

    connect(ui_->searchLineEdit, &QLineEdit::textChanged, this,
            &DatasetsListBrowser::searchTextChanged);

    setupDatasetsList();
}

void DatasetsListBrowser::searchTextChanged(const QString& arg1) const
{
    const int count{ui_->datasetsList->count()};
    for (int i{0}; i < count; ++i)
    {
        QListWidgetItem* item{ui_->datasetsList->item(i)};
        const bool hide{!item->text().contains(arg1, Qt::CaseInsensitive)};
        item->setHidden(hide);
    }
}

void DatasetsListBrowser::clearSelection()
{
    ui_->datasetsList->clearSelection();
}

bool DatasetsListBrowser::isDatasetsListEmpty() const
{
    return (ui_->datasetsList->count() == 0);
}

void DatasetsListBrowser::setupDatasetsList()
{
    ui_->datasetsList->insertItems(
        0, dataset_utilities::getListOfAvailableDatasets());

    ui_->datasetsList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui_->datasetsList, &QListWidget::customContextMenuRequested, this,
            &DatasetsListBrowser::showContextMenu);

    connect(ui_->datasetsList, &QListWidget::itemSelectionChanged, this,
            &DatasetsListBrowser::datasetListItemSelectionChanged);
}

bool DatasetsListBrowser::userWantToDeleteDataset(QPoint pos) const
{
    const QPoint globalPos{ui_->datasetsList->viewport()->mapToGlobal(pos)};

    if (ui_->datasetsList->selectedItems().isEmpty() ||
        (ui_->datasetsList->itemAt(pos) == nullptr))
        return false;

    const QIcon deleteIcon{
        QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton)};

    QMenu myMenu;
    myMenu.addAction(deleteIcon, QObject::tr("Delete dataset"));

    const QAction* selectedItem{myMenu.exec(globalPos)};

    return selectedItem != nullptr;
}

bool DatasetsListBrowser::userConfirmedDeleting(const QString& datasetToDelete)
{
    const QMessageBox::StandardButton answer{QMessageBox::question(
        this, tr("Delete?"), tr("Delete dataset ") + datasetToDelete + "?",
        QMessageBox::Yes | QMessageBox::No)};

    return QMessageBox::Yes == answer;
}

void DatasetsListBrowser::deleteSelectedDataset(const QString& datasetToDelete)
{
    if (!dataset_utilities::removeDataset(datasetToDelete))
        QMessageBox::warning(this, tr("Error"),
                             tr("Can not delete ") + datasetToDelete + ".");

    ui_->datasetsList->clear();
    ui_->datasetsList->insertItems(
        0, dataset_utilities::getListOfAvailableDatasets());
}

void DatasetsListBrowser::showContextMenu(QPoint pos)
{
    if (ui_->datasetsList->selectedItems().isEmpty())
        return;

    const QString datasetToDelete{
        ui_->datasetsList->selectedItems().first()->text()};

    if (userWantToDeleteDataset(pos) && userConfirmedDeleting(datasetToDelete))
        deleteSelectedDataset(datasetToDelete);
}

void DatasetsListBrowser::datasetListItemSelectionChanged()
{
    QString newCurrent{QLatin1String("")};

    if (QList<QListWidgetItem*> items{ui_->datasetsList->selectedItems()};
        !items.isEmpty())
        newCurrent = items.front()->text();

    emit currentDatasetChanged(newCurrent);
}
