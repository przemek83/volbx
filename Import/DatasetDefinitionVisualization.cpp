#include "DatasetDefinitionVisualization.h"

#include <QDebug>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>

#include "Common/Constants.h"
#include "Datasets/DatasetDefinition.h"

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

    connect(ui->searchLineEdit, &QLineEdit::textChanged,
            this, &DatasetDefinitionVisualization::searchTextChanged);

    connect(ui->columnsList, &QTreeWidget::currentItemChanged,
            this, &DatasetDefinitionVisualization::currentColumnOnTreeChanged);

    connect(ui->dateCombo,  qOverload<int>(&QComboBox::currentIndexChanged),
            this, &DatasetDefinitionVisualization::specialColumnChanged);

    connect(ui->pricePerUnitCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &DatasetDefinitionVisualization::specialColumnChanged);

    connect(ui->SelectAll, &QPushButton::clicked,
            this, &DatasetDefinitionVisualization::selectAllClicked);

    connect(ui->UnselectAll, &QPushButton::clicked,
            this, &DatasetDefinitionVisualization::unselectAllClicked);
}

DatasetDefinitionVisualization::~DatasetDefinitionVisualization()
{
    delete ui;
}

void DatasetDefinitionVisualization::setDatasetDefiniton(
    std::unique_ptr<DatasetDefinition> datasetDefinition)
{
    clear();

    ui->dateCombo->blockSignals(true);
    ui->pricePerUnitCombo->blockSignals(true);

    datasetDefinition_ = std::move(datasetDefinition);

    auto [dateOfTransactionPointed, specialColumnTransaction] =
        datasetDefinition_->getSpecialColumnIfExists(SPECIAL_COLUMN_TRANSACTION_DATE);

    auto [pricePerUnitPointed, specialColumnPrice] =
        datasetDefinition_->getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT);

    ui->columnsList->sortByColumn(Constants::NOT_SET_COLUMN);
    ui->columnsList->setSortingEnabled(false);

    //Column list.
    for (int i = 0; i < datasetDefinition_->columnCount(); ++i)
    {
        QStringList list;
        list << datasetDefinition_->getColumnName(i);
        QString typeName(QLatin1String(""));
        switch (datasetDefinition_->getColumnFormat(i))
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

            case DATA_FORMAT_UNKNOWN:
            {
                Q_ASSERT(false);
                break;
            }
        }

        list << typeName;

        auto item = new QTreeWidgetItem(list);
        item->setData(0, Qt::UserRole, QVariant(i));
        ui->columnsList->addTopLevelItem(item);
    }

    ui->columnsList->header()->resizeSections(QHeaderView::ResizeToContents);

    ui->columnsList->setSortingEnabled(true);
    ui->columnsList->sortByColumn(Constants::NOT_SET_COLUMN);

    //Set proper special columns.
    if (dateOfTransactionPointed)
    {
        for (int i = 0; i < ui->dateCombo->count(); ++i)
        {
            if (specialColumnTransaction == ui->dateCombo->itemData(i).toInt())
            {
                ui->dateCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    if (pricePerUnitPointed)
    {
        for (int i = 0; i < ui->pricePerUnitCombo->count(); ++i)
        {
            if (specialColumnPrice == ui->pricePerUnitCombo->itemData(i).toInt())
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

    datasetDefinition_ = nullptr;
}

void DatasetDefinitionVisualization::searchTextChanged(const QString& newText)
{
    QTreeWidgetItemIterator it(ui->columnsList);
    while (*it != nullptr)
    {
        (*it)->setHidden(!(*it)->text(0).contains(newText,
                                                  Qt::CaseInsensitive));
        ++it;
    }
}

std::unique_ptr<DatasetDefinition> DatasetDefinitionVisualization::retrieveDatasetDefinition()
{
    if (datasetDefinition_ == nullptr)
    {
        return nullptr;
    }

    QVector<bool> activeColumns = QVector<bool>();

    int topLevelItemsCount =  ui->columnsList->topLevelItemCount();
    activeColumns.resize(topLevelItemsCount);

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);

        bool active = false;

        if (currentLoopItem->flags().testFlag(Qt::ItemIsUserCheckable))
        {
            active = (currentLoopItem->checkState(0) == Qt::Checked);
        }
        else
        {
            active = true;
        }

        activeColumns[currentLoopItem->data(0, Qt::UserRole).toInt()] = active;
    }

    datasetDefinition_->setActiveColumns(activeColumns);

    if (ui->dateCombo->currentIndex() != -1)
    {
        int column =
            ui->dateCombo->itemData(ui->dateCombo->currentIndex()).toInt();
        datasetDefinition_->setSpecialColumn(SPECIAL_COLUMN_TRANSACTION_DATE,
                                             column);

    }

    if (ui->pricePerUnitCombo->currentIndex() != -1)
    {
        int index = ui->pricePerUnitCombo->currentIndex();
        int column = ui->pricePerUnitCombo->itemData(index).toInt();
        datasetDefinition_->setSpecialColumn(SPECIAL_COLUMN_PRICE_PER_UNIT,
                                             column);

    }

    return std::move(datasetDefinition_);
}


void DatasetDefinitionVisualization::currentColumnOnTreeChanged(
    QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/)
{
    if (nullptr == current)
    {
        return;
    }

    Q_EMIT currentColumnNeedSync(current->data(0, Qt::UserRole).toInt());
}

void DatasetDefinitionVisualization::selectCurrentColumn(int column)
{
    QList<QTreeWidgetItem*> selectedItemsList = ui->columnsList->selectedItems();
    if (0 != selectedItemsList.count() &&
        column == selectedItemsList.first()->data(0, Qt::UserRole).toInt())
    {
        return;
    }

    int topLevelItemsCount =  ui->columnsList->topLevelItemCount();

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);
        if (column == currentLoopItem->data(0, Qt::UserRole).toInt())
        {
            ui->columnsList->setCurrentItem(currentLoopItem);
            break;
        }
    }
}

void DatasetDefinitionVisualization::selectAllClicked()
{
    int topLevelItemsCount =  ui->columnsList->topLevelItemCount();

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);
        if (currentLoopItem->flags().testFlag(Qt::ItemIsUserCheckable))
        {
            currentLoopItem->setCheckState(0, Qt::Checked);
        }
    }
}

void DatasetDefinitionVisualization::unselectAllClicked()
{
    int topLevelItemsCount =  ui->columnsList->topLevelItemCount();

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);
        if (currentLoopItem->flags().testFlag(Qt::ItemIsUserCheckable))
        {
            currentLoopItem->setCheckState(0, Qt::Unchecked);
        }
    }
}

void DatasetDefinitionVisualization::specialColumnChanged(int /*newIndex*/)
{
    int dateColumn = Constants::NOT_SET_COLUMN;
    if (ui->dateCombo->currentIndex() != -1)
    {
        dateColumn =
            ui->dateCombo->itemData(ui->dateCombo->currentIndex()).toInt();
    }

    int priceColumn = Constants::NOT_SET_COLUMN;
    if (ui->pricePerUnitCombo->currentIndex() != -1)
    {
        int index = ui->pricePerUnitCombo->currentIndex();
        priceColumn = ui->pricePerUnitCombo->itemData(index).toInt();
    }

    int topLevelItemsCount = ui->columnsList->topLevelItemCount();

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem = ui->columnsList->topLevelItem(i);
        int itemColumn = currentLoopItem->data(0, Qt::UserRole).toInt();
        if (priceColumn == itemColumn || dateColumn == itemColumn)
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
