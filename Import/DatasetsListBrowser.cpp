#include <QMenu>
#include <QMessageBox>

#include "Datasets/DatasetInner.h"
#include "Common/Constants.h"

#include "DatasetsListBrowser.h"
#include "ui_DatasetsListBrowser.h"

DatasetsListBrowser::DatasetsListBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatasetsListBrowser)
{
    ui->setupUi(this);

    connect(ui->searchLineEdit,
            SIGNAL(textChanged(QString)),
            this,
            SLOT(searchTextChanged(QString)));

    ui->datasetsList->insertItems(0, DatasetInner::getListOfAvailableDatasets());

    ui->datasetsList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->datasetsList, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
}

DatasetsListBrowser::~DatasetsListBrowser()
{
    delete ui;
}

QString DatasetsListBrowser::getSelectedDataset() const
{
    QListWidgetItem* item = ui->datasetsList->currentItem();
    if(item) {
        return item->text();
}
    return QString();
}

void DatasetsListBrowser::searchTextChanged(const QString &arg1)
{
    for(int i = 0; i < ui->datasetsList->count(); ++i)
    {
        QListWidgetItem* item = ui->datasetsList->item(i);
        item->setHidden(false == item->text().contains(arg1, Qt::CaseInsensitive));
    }
}

void DatasetsListBrowser::clearSelection()
{
    ui->datasetsList->clearSelection();
}

bool DatasetsListBrowser::isDatasetsListEmpty()
{
    return (0 == ui->datasetsList->count());
}

void DatasetsListBrowser::showContextMenu(const QPoint& pos)
{
    //Create delete context menu.
    QPoint globalPos = ui->datasetsList->viewport()->mapToGlobal(pos);

    if ( true == ui->datasetsList->selectedItems().isEmpty() ||
         nullptr == ui->datasetsList->itemAt(pos))
    {
        return;
    }

    static QIcon deleteIcon =
        QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton);

    QMenu myMenu;
    myMenu.addAction(deleteIcon, QObject::tr("Delete dataset"));

    QAction* selectedItem = myMenu.exec(globalPos);

    //Delete dataset.
    if ( nullptr != selectedItem )
    {
        QString datasetName = ui->datasetsList->selectedItems().first()->text();
        QMessageBox::StandardButton answer =
                QMessageBox::question(this,
                                      tr("Delete?"),
                                      tr("Delete dataset ") + datasetName + "?",
                                      QMessageBox::Yes | QMessageBox::No);

        if ( QMessageBox::Yes == answer )
        {
            if ( false == DatasetInner::removeDataset(datasetName) )
            {
                QMessageBox::warning(this,
                                     tr("Error"),
                                     tr("Can not delete ") + datasetName + ".");
            }

            ui->datasetsList->clear();
            ui->datasetsList->insertItems(0, DatasetInner::getListOfAvailableDatasets());
        }
    }
}

void DatasetsListBrowser::on_datasetsList_itemSelectionChanged()
{
    QString newCurrent("");
    QList<QListWidgetItem*> selectedItems = ui->datasetsList->selectedItems();
    if( false == selectedItems.isEmpty() ) {
        newCurrent = selectedItems.at(0)->text();
}

    Q_EMIT currentDatasetChanged(newCurrent);
}
