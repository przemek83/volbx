#include "DatasetVisualization.h"

#include <Common/Constants.h>
#include <Datasets/Dataset.h>

#include "ui_DatasetVisualization.h"

DatasetVisualization::DatasetVisualization(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::DatasetVisualization),
      typeNameString_(tr("Name")),
      typeNameFloat_(tr("Number")),
      typeNameDate_(tr("Date"))
{
    ui->setupUi(this);

    ui->columnsList->header()->setSectionsMovable(false);

    connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
            &DatasetVisualization::searchTextChanged);

    connect(ui->columnsList, &QTreeWidget::currentItemChanged, this,
            &DatasetVisualization::currentColumnOnTreeChanged);

    connect(ui->dateCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &DatasetVisualization::taggedColumnChanged);

    connect(ui->pricePerUnitCombo,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &DatasetVisualization::taggedColumnChanged);

    connect(ui->SelectAll, &QPushButton::clicked, this,
            &DatasetVisualization::selectAllClicked);

    connect(ui->UnselectAll, &QPushButton::clicked, this,
            &DatasetVisualization::unselectAllClicked);
}

DatasetVisualization::~DatasetVisualization() { delete ui; }

void DatasetVisualization::setDataset(std::unique_ptr<Dataset> dataset)
{
    clear();

    ui->dateCombo->blockSignals(true);
    ui->pricePerUnitCombo->blockSignals(true);

    dataset_ = std::move(dataset);

    auto [dateOfTransactionPointed, dateColumn] =
        dataset_->getTaggedColumn(ColumnTag::DATE);

    auto [pricePerUnitPointed, valueColumn] =
        dataset_->getTaggedColumn(ColumnTag::VALUE);

    ui->columnsList->sortByColumn(Constants::NOT_SET_COLUMN);
    ui->columnsList->setSortingEnabled(false);

    // Column list.
    for (unsigned int i = 0; i < dataset_->columnCount(); ++i)
    {
        QStringList list;
        list << dataset_->getHeaderName(i);
        QString typeName{QLatin1String("")};
        switch (dataset_->getColumnFormat(i))
        {
            case ColumnType::STRING:
            {
                typeName = QString(typeNameString_);
                break;
            }

            case ColumnType::NUMBER:
            {
                typeName = QString(typeNameFloat_);
                ui->pricePerUnitCombo->addItem(dataset_->getHeaderName(i),
                                               QVariant(i));
                break;
            }

            case ColumnType::DATE:
            {
                typeName = QString(typeNameDate_);
                ui->dateCombo->addItem(dataset_->getHeaderName(i), QVariant(i));
                break;
            }

            case ColumnType::UNKNOWN:
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

    // Set proper tagged columns.
    if (dateOfTransactionPointed)
    {
        for (int i = 0; i < ui->dateCombo->count(); ++i)
        {
            if (dateColumn == ui->dateCombo->itemData(i).toUInt())
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
            if (valueColumn == ui->pricePerUnitCombo->itemData(i).toUInt())
            {
                ui->pricePerUnitCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    ui->columnsList->setEnabled(true);

    ui->taggedColumnsWidget->setEnabled(true);

    ui->dateCombo->blockSignals(false);
    ui->pricePerUnitCombo->blockSignals(false);

    // Set on tree picked tagged columns in combos.
    taggedColumnChanged(0);
}

void DatasetVisualization::clear()
{
    ui->pricePerUnitCombo->clear();
    ui->dateCombo->clear();

    ui->columnsList->clear();

    ui->columnsList->setEnabled(false);

    ui->taggedColumnsWidget->setEnabled(false);

    dataset_ = nullptr;
}

void DatasetVisualization::searchTextChanged(const QString& newText)
{
    QTreeWidgetItemIterator it(ui->columnsList);
    while (*it != nullptr)
    {
        (*it)->setHidden(
            !(*it)->text(0).contains(newText, Qt::CaseInsensitive));
        ++it;
    }
}

std::unique_ptr<Dataset> DatasetVisualization::retrieveDataset()
{
    if (dataset_ == nullptr)
        return nullptr;

    QVector<bool> activeColumns;

    int topLevelItemsCount{ui->columnsList->topLevelItemCount()};
    activeColumns.resize(topLevelItemsCount);

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem{ui->columnsList->topLevelItem(i)};

        bool active{false};

        if (currentLoopItem->flags().testFlag(Qt::ItemIsUserCheckable))
            active = (currentLoopItem->checkState(0) == Qt::Checked);
        else
            active = true;

        activeColumns[currentLoopItem->data(0, Qt::UserRole).toInt()] = active;
    }

    dataset_->setActiveColumns(activeColumns);

    if (ui->dateCombo->currentIndex() != -1)
    {
        int column{
            ui->dateCombo->itemData(ui->dateCombo->currentIndex()).toInt()};
        dataset_->setTaggedColumn(ColumnTag::DATE, column);
    }

    if (ui->pricePerUnitCombo->currentIndex() != -1)
    {
        int index{ui->pricePerUnitCombo->currentIndex()};
        int column{ui->pricePerUnitCombo->itemData(index).toInt()};
        dataset_->setTaggedColumn(ColumnTag::VALUE, column);
    }

    return std::move(dataset_);
}

void DatasetVisualization::currentColumnOnTreeChanged(
    QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/)
{
    if (current == nullptr)
        return;

    Q_EMIT currentColumnNeedSync(current->data(0, Qt::UserRole).toInt());
}

void DatasetVisualization::selectCurrentColumn(int column)
{
    QList<QTreeWidgetItem*> selectedItemsList{ui->columnsList->selectedItems()};
    if (0 != selectedItemsList.count() &&
        column == selectedItemsList.first()->data(0, Qt::UserRole).toInt())
        return;

    int topLevelItemsCount{ui->columnsList->topLevelItemCount()};

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem{ui->columnsList->topLevelItem(i)};
        if (column == currentLoopItem->data(0, Qt::UserRole).toInt())
        {
            ui->columnsList->setCurrentItem(currentLoopItem);
            break;
        }
    }
}

void DatasetVisualization::selectAllClicked()
{
    int topLevelItemsCount{ui->columnsList->topLevelItemCount()};

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem{ui->columnsList->topLevelItem(i)};
        if (currentLoopItem->flags().testFlag(Qt::ItemIsUserCheckable))
            currentLoopItem->setCheckState(0, Qt::Checked);
    }
}

void DatasetVisualization::unselectAllClicked()
{
    const int topLevelItemsCount{ui->columnsList->topLevelItemCount()};

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem{ui->columnsList->topLevelItem(i)};
        if (currentLoopItem->flags().testFlag(Qt::ItemIsUserCheckable))
            currentLoopItem->setCheckState(0, Qt::Unchecked);
    }
}

void DatasetVisualization::taggedColumnChanged(int /*newIndex*/)
{
    int dateColumn{Constants::NOT_SET_COLUMN};
    if (ui->dateCombo->currentIndex() != -1)
        dateColumn =
            ui->dateCombo->itemData(ui->dateCombo->currentIndex()).toInt();

    int priceColumn{Constants::NOT_SET_COLUMN};
    if (ui->pricePerUnitCombo->currentIndex() != -1)
    {
        int index = ui->pricePerUnitCombo->currentIndex();
        priceColumn = ui->pricePerUnitCombo->itemData(index).toInt();
    }

    int topLevelItemsCount{ui->columnsList->topLevelItemCount()};

    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentLoopItem{ui->columnsList->topLevelItem(i)};
        int itemColumn{currentLoopItem->data(0, Qt::UserRole).toInt()};
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
