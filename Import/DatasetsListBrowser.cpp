#include "DatasetsListBrowser.h"

#include <QMenu>
#include <QMessageBox>

#include <Common/DatasetUtilities.h>

#include "ui_DatasetsListBrowser.h"

DatasetsListBrowser::DatasetsListBrowser(QWidget* parent)
    : QWidget(parent), ui(new Ui::DatasetsListBrowser)
{
    ui->setupUi(this);

    connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
            &DatasetsListBrowser::searchTextChanged);

    setupDatasetsList();
}

DatasetsListBrowser::~DatasetsListBrowser() { delete ui; }

void DatasetsListBrowser::searchTextChanged(const QString& arg1)
{
    for (int i = 0; i < ui->datasetsList->count(); ++i)
    {
        QListWidgetItem* item{ui->datasetsList->item(i)};
        const bool hide{!item->text().contains(arg1, Qt::CaseInsensitive)};
        item->setHidden(hide);
    }
}

void DatasetsListBrowser::clearSelection()
{
    ui->datasetsList->clearSelection();
}

bool DatasetsListBrowser::isDatasetsListEmpty() const
{
    return (ui->datasetsList->count() == 0);
}

void DatasetsListBrowser::setupDatasetsList()
{
    ui->datasetsList->insertItems(
        0, DatasetUtilities::getListOfAvailableDatasets());

    ui->datasetsList->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->datasetsList, &QListWidget::customContextMenuRequested, this,
            &DatasetsListBrowser::showContextMenu);

    connect(ui->datasetsList, &QListWidget::itemSelectionChanged, this,
            &DatasetsListBrowser::datasetsListItemSelectionChanged);
}

bool DatasetsListBrowser::doesUserChooseToDeleteSelectedDataset(QPoint pos)
{
    const QPoint globalPos{ui->datasetsList->viewport()->mapToGlobal(pos)};

    if (ui->datasetsList->selectedItems().isEmpty() ||
        ui->datasetsList->itemAt(pos) == nullptr)
        return false;

    const QIcon deleteIcon{
        QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton)};

    QMenu myMenu;
    myMenu.addAction(deleteIcon, QObject::tr("Delete dataset"));

    QAction* selectedItem{myMenu.exec(globalPos)};

    return selectedItem != nullptr;
}

bool DatasetsListBrowser::doesUserConfirmedDeleting(
    const QString& datasetToDelete)
{
    const QMessageBox::StandardButton answer{QMessageBox::question(
        this, tr("Delete?"), tr("Delete dataset ") + datasetToDelete + "?",
        QMessageBox::Yes | QMessageBox::No)};

    return QMessageBox::Yes == answer;
}

void DatasetsListBrowser::deleteSelectedDataset(const QString& datasetToDelete)
{
    if (!DatasetUtilities::removeDataset(datasetToDelete))
        QMessageBox::warning(this, tr("Error"),
                             tr("Can not delete ") + datasetToDelete + ".");

    ui->datasetsList->clear();
    ui->datasetsList->insertItems(
        0, DatasetUtilities::getListOfAvailableDatasets());
}

void DatasetsListBrowser::showContextMenu(QPoint pos)
{
    if (ui->datasetsList->selectedItems().isEmpty())
        return;

    const QString datasetToDelete{
        ui->datasetsList->selectedItems().first()->text()};

    if (doesUserChooseToDeleteSelectedDataset(pos) &&
        doesUserConfirmedDeleting(datasetToDelete))
        deleteSelectedDataset(datasetToDelete);
}

void DatasetsListBrowser::datasetsListItemSelectionChanged()
{
    QString newCurrent{QStringLiteral("")};
    QList<QListWidgetItem*> selectedItems{ui->datasetsList->selectedItems()};
    if (!selectedItems.isEmpty())
        newCurrent = selectedItems.front()->text();

    Q_EMIT currentDatasetChanged(newCurrent);
}
