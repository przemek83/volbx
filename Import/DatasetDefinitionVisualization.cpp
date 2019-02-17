#include <QDebug>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>

#include "Datasets/DatasetDefinition.h"
#include "Common/Constants.h"

#include "DatasetDefinitionVisualization.h"
#include "ui_DatasetDefinitionVisualization.h"

DatasetDefinitionVisualization::DatasetDefinitionVisualization(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DatasetDefinitionVisualization),
    typeNameString_(tr("Name")),
    typeNameFloat_(tr("Number")),
    typeNameDate_(tr("Date")),
    datasetDefinition_(nullptr)
{
    ui->setupUi(this);

    ui->columnsList->header()->setSectionsMovable(false);

    connect(ui->searchLineEdit,
            SIGNAL(textChanged(QString)),
            this,
            SLOT(searchTextChanged(QString)));

    connect(ui->columnsList,
            SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this,
            SLOT(currentColumnOnTreeChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

    connect(ui->dateCombo,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(specialColumnChanged(int)));

    connect(ui->pricePerUnitCombo,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(specialColumnChanged(int)));
}

DatasetDefinitionVisualization::~DatasetDefinitionVisualization()
{
    delete ui;
}

void DatasetDefinitionVisualization::setDatasetDefiniton(
        DatasetDefinition* datasetDefinition)
{
    clear();

    ui->dateCombo->blockSignals(true);
    ui->pricePerUnitCombo->blockSignals(true);

    datasetDefinition_ = datasetDefinition;

    int specialColumnTransaction = -1;
    bool dateOfTransactionPointed =
        datasetDefinition_->getSpecialColumnIfExists(SPECIAL_COLUMN_TRANSACTION_DATE,
                                                     specialColumnTransaction);

    int specialColumnPrice = -1;
    bool pricePerUnitPointed =
        datasetDefinition_->getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT,
                                                     specialColumnPrice);
    ui->columnsList->sortByColumn(-1);
    ui->columnsList->setSortingEnabled(false);

    //Column list.
    for(int i = 0; i < datasetDefinition_->columnCount(); ++i)
    {
        QStringList list;
        list << datasetDefinition_->getColumnName(i);
        QString typeName("");
        switch(datasetDefinition_->getColumnFormat(i))
        {
            case DATA_FORMAT_STRING:
            {
                typeName = QString(typeNameString_);
                break;
            }

            case DATA_FORMAT_FLOAT:
            {
                typeName = QString(typeNameFloat_);
                ui->pricePerUnitCombo->addItem(datasetDefinition_->getColumnName(i),
                                               QVariant(i));
                break;
            }

            case DATA_FORMAT_DATE:
            {
                typeName = QString(typeNameDate_);
                ui->dateCombo->addItem(datasetDefinition_->getColumnName(i),
                                       QVariant(i));
                break;
            }

            default:
            {
                Q_ASSERT(false);
                break;
            }
        }

        list << typeName;

        QTreeWidgetItem* item = new QTreeWidgetItem(list);
        item->setData(0, Qt::UserRole, QVariant(i));
        ui->columnsList->addTopLevelItem(item);
    }

    ui->columnsList->header()->resizeSections(QHeaderView::ResizeToContents);

    ui->columnsList->setSortingEnabled(true);
    ui->columnsList->sortByColumn(-1);

    //Set proper special columns.
    if ( true == dateOfTransactionPointed )
    {
        for(int i = 0; i < ui->dateCombo->count(); ++i)
        {
            if(specialColumnTransaction == ui->dateCombo->itemData(i).toInt())
            {
                ui->dateCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    if ( pricePerUnitPointed )
    {
        for(int i = 0; i < ui->pricePerUnitCombo->count(); ++i)
        {
            if(specialColumnPrice == ui->pricePerUnitCombo->itemData(i).toInt())
            {
                ui->pricePerUnitCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    ui->columnsList->setEnabled(true);

    ui->specialColumnsWidget->setEnabled(true);

    ui->dateCombo->blockSignals(false);
    ui->pricePerUnitCombo->blockSignals(false);

    //Set on tree picked special columns in combos.
    specialColumnChanged(0);
}

void DatasetDefinitionVisualization::clear()
{
    ui->pricePerUnitCombo->clear();
    ui->dateCombo->clear();

    ui->columnsList->clear();

    ui->columnsList->setEnabled(false);

    ui->specialColumnsWidget->setEnabled(false);

    if ( nullptr != datasetDefinition_ )
    {
        delete datasetDefinition_;
        datasetDefinition_ = nullptr;
    }
}

void DatasetDefinitionVisualization::searchTextChanged(const QString& newText)
{
    QTreeWidgetItemIterator it(ui->columnsList);
    while (*it)
    {
        (*it)->setHidden(false == (*it)->text(0).contains(newText,
                                                          Qt::CaseInsensitive));
        ++it;
    }
}

DatasetDefinition* DatasetDefinitionVisualization::getDatasetDefinition()
{
    QVector<bool> activeColumns = QVector<bool>();

    int topLevelItemsCount =  ui->columnsList->topLevelItemCount();
    activeColumns.resize(topLevelItemsCount);

    for ( int i = 0; i < topLevelItemsCount; ++i )
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);

        bool active = false;

        if ( currentLoopItem->flags() & Qt::ItemIsUserCheckable )
        {
            active =
                ( currentLoopItem->checkState(0) == Qt::Checked ? true : false );
        }
        else
        {
            active = true;
        }

        activeColumns[currentLoopItem->data(0, Qt::UserRole).toInt()] = active;
    }

    datasetDefinition_->setActiveColumns(activeColumns);

    if ( ui->dateCombo->currentIndex() != -1 )
    {
        int column =
            ui->dateCombo->itemData(ui->dateCombo->currentIndex()).toInt();
        datasetDefinition_->setSpecialColumn(SPECIAL_COLUMN_TRANSACTION_DATE,
                                             column);

    }

    if ( ui->pricePerUnitCombo->currentIndex() != -1 )
    {
        int index = ui->pricePerUnitCombo->currentIndex();
        int column = ui->pricePerUnitCombo->itemData(index).toInt();
        datasetDefinition_->setSpecialColumn(SPECIAL_COLUMN_PRICE_PER_UNIT,
                                             column);

    }

    return datasetDefinition_;
}


void DatasetDefinitionVisualization::currentColumnOnTreeChanged(
        QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/)
{
    if ( nullptr == current )
        return;

    emit currentColumnNeedSync(current->data(0, Qt::UserRole).toInt());
}

void DatasetDefinitionVisualization::selectCurrentColumn(int column)
{
    QList<QTreeWidgetItem*> selectedItemsList = ui->columnsList->selectedItems();
    if ( 0 != selectedItemsList.count() &&
         column == selectedItemsList.first()->data(0, Qt::UserRole).toInt() )
    {
        return;
    }

    int topLevelItemsCount =  ui->columnsList->topLevelItemCount();

    for ( int i = 0; i < topLevelItemsCount; ++i )
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);
        if ( column == currentLoopItem->data(0, Qt::UserRole).toInt() )
        {
            ui->columnsList->setCurrentItem(currentLoopItem);
            break;
        }
    }
}

void DatasetDefinitionVisualization::on_SelectAll_clicked()
{
    int topLevelItemsCount =  ui->columnsList->topLevelItemCount();

    for ( int i = 0; i < topLevelItemsCount; ++i )
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);
        if ( currentLoopItem->flags() & Qt::ItemIsUserCheckable )
        {
            currentLoopItem->setCheckState(0, Qt::Checked);
        }
    }
}

void DatasetDefinitionVisualization::on_UnselectAll_clicked()
{
    int topLevelItemsCount =  ui->columnsList->topLevelItemCount();

    for ( int i = 0; i < topLevelItemsCount; ++i )
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);
        if ( currentLoopItem->flags() & Qt::ItemIsUserCheckable )
        {
            currentLoopItem->setCheckState(0, Qt::Unchecked);
        }
    }
}

void DatasetDefinitionVisualization::specialColumnChanged(int /*newIndex*/)
{
    int dateColumn = -1;
    if( ui->dateCombo->currentIndex() != -1 )
    {
        dateColumn =
            ui->dateCombo->itemData(ui->dateCombo->currentIndex()).toInt();
    }

    int priceColumn = -1;
    if( ui->pricePerUnitCombo->currentIndex() != -1 )
    {
        int index = ui->pricePerUnitCombo->currentIndex();
        priceColumn = ui->pricePerUnitCombo->itemData(index).toInt();
    }

    int topLevelItemsCount = ui->columnsList->topLevelItemCount();

    for ( int i = 0; i < topLevelItemsCount; ++i )
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);
        int itemColumn = currentLoopItem->data(0, Qt::UserRole).toInt();
        if ( priceColumn == itemColumn || dateColumn == itemColumn )
        {
            currentLoopItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            currentLoopItem->setData(0, Qt::CheckStateRole, QVariant());
        }
        else
        {
            currentLoopItem->setFlags(Qt::ItemIsSelectable |
                                      Qt::ItemIsUserCheckable |
                                      Qt::ItemIsEnabled);
            currentLoopItem->setCheckState(0, Qt::Checked);
        }
    }
}
