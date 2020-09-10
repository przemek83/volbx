#include "DatasetVisualization.h"

#include <Common/Constants.h>
#include <Datasets/Dataset.h>

#include "ui_DatasetVisualization.h"

DatasetVisualization::DatasetVisualization(QWidget* parent)
    : QWidget(parent), ui(new Ui::DatasetVisualization)
{
    ui->setupUi(this);

    ui->columnsList->header()->setSectionsMovable(false);

    connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
            &DatasetVisualization::searchTextChanged);

    connect(ui->columnsList, &QTreeWidget::currentItemChanged, this,
            &DatasetVisualization::currentColumnOnTreeChanged);

    connect(ui->dateCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &DatasetVisualization::refreshColumnList);

    connect(ui->pricePerUnitCombo,
            qOverload<int>(&QComboBox::currentIndexChanged), this,
            &DatasetVisualization::refreshColumnList);

    connect(ui->SelectAll, &QPushButton::clicked, this,
            &DatasetVisualization::selectAllClicked);

    connect(ui->UnselectAll, &QPushButton::clicked, this,
            &DatasetVisualization::unselectAllClicked);
}

DatasetVisualization::~DatasetVisualization() { delete ui; }

void DatasetVisualization::setDataset(std::unique_ptr<Dataset> dataset)
{
    clear();

    dataset_ = std::move(dataset);

    setupColumnsListWidget();

    fillTaggedColumnCombos();

    setTaggedColumns();

    ui->taggedColumnsWidget->setEnabled(true);

    refreshColumnList(0);
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
        const QString& currentItemText{(*it)->text(0)};
        (*it)->setHidden(
            !currentItemText.contains(newText, Qt::CaseInsensitive));
        ++it;
    }
}

std::unique_ptr<Dataset> DatasetVisualization::retrieveDataset()
{
    if (dataset_ == nullptr)
        return nullptr;

    dataset_->setActiveColumns(getActiveColumns());

    setTaggedColumnInDataset(ColumnTag::DATE, ui->dateCombo);
    setTaggedColumnInDataset(ColumnTag::VALUE, ui->pricePerUnitCombo);

    return std::move(dataset_);
}

void DatasetVisualization::currentColumnOnTreeChanged(
    QTreeWidgetItem* current, [[maybe_unused]] QTreeWidgetItem* previous)
{
    if (current == nullptr)
        return;

    Q_EMIT currentColumnNeedSync(current->data(0, Qt::UserRole).toInt());
}

void DatasetVisualization::selectCurrentColumn(int column)
{
    const QList<QTreeWidgetItem*> selectedItemsList{
        ui->columnsList->selectedItems()};
    if (selectedItemsList.count() != 0 &&
        selectedItemsList.first()->data(0, Qt::UserRole).toInt() == column)
        return;

    for (int i = 0; i < ui->columnsList->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* currentItem{ui->columnsList->topLevelItem(i)};
        if (currentItem->data(0, Qt::UserRole).toInt() == column)
        {
            ui->columnsList->setCurrentItem(currentItem);
            break;
        }
    }
}

void DatasetVisualization::setCurrentIndexUsingColumn(QComboBox* combo,
                                                      int column)
{
    for (int i = 0; i < combo->count(); ++i)
    {
        if (combo->itemData(i).toInt() == column)
        {
            combo->setCurrentIndex(i);
            break;
        }
    }
}

void DatasetVisualization::setupColumnsListWidget()
{
    ui->columnsList->sortByColumn(Constants::NOT_SET_COLUMN);
    ui->columnsList->setSortingEnabled(false);

    for (int column = 0; column < static_cast<int>(dataset_->columnCount());
         ++column)
    {
        const QStringList list{dataset_->getHeaderName(column),
                               getTypeDisplayNameForGivenColumn(column)};
        auto* item{new QTreeWidgetItem(list)};
        item->setData(0, Qt::UserRole, QVariant(column));
        ui->columnsList->addTopLevelItem(item);
    }

    ui->columnsList->header()->resizeSections(QHeaderView::ResizeToContents);

    ui->columnsList->setSortingEnabled(true);
    ui->columnsList->sortByColumn(Constants::NOT_SET_COLUMN);

    ui->columnsList->setEnabled(true);
}

void DatasetVisualization::setTaggedColumns()
{
    ui->dateCombo->blockSignals(true);
    ui->pricePerUnitCombo->blockSignals(true);

    auto [dateOfTransactionPointed, dateColumn] =
        dataset_->getTaggedColumn(ColumnTag::DATE);
    if (dateOfTransactionPointed)
        setCurrentIndexUsingColumn(ui->dateCombo, dateColumn);

    auto [pricePerUnitPointed, valueColumn] =
        dataset_->getTaggedColumn(ColumnTag::VALUE);
    if (pricePerUnitPointed)
        setCurrentIndexUsingColumn(ui->pricePerUnitCombo, valueColumn);

    ui->dateCombo->blockSignals(false);
    ui->pricePerUnitCombo->blockSignals(false);
}

QVector<bool> DatasetVisualization::getActiveColumns() const
{
    int topLevelItemsCount{ui->columnsList->topLevelItemCount()};
    QVector<bool> activeColumns;
    activeColumns.resize(topLevelItemsCount);
    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentItem{ui->columnsList->topLevelItem(i)};
        bool active{false};
        if (currentItem->flags().testFlag(Qt::ItemIsUserCheckable))
            active = (currentItem->checkState(0) == Qt::Checked);
        else
            active = true;
        activeColumns[currentItem->data(0, Qt::UserRole).toInt()] = active;
    }

    return activeColumns;
}

void DatasetVisualization::setTaggedColumnInDataset(ColumnTag tag,
                                                    QComboBox* combo)
{
    if (combo->currentIndex() == -1)
        return;

    const int column{combo->itemData(combo->currentIndex()).toInt()};
    dataset_->setTaggedColumn(tag, column);
}

QString DatasetVisualization::getTypeDisplayNameForGivenColumn(int column) const
{
    ColumnType columnType{dataset_->getColumnFormat(column)};
    if (columnType == ColumnType::STRING)
        return typeNameString_;
    if (columnType == ColumnType::NUMBER)
        return typeNameFloat_;
    if (columnType == ColumnType::DATE)
        return typeNameDate_;

    Q_ASSERT(false);
}

void DatasetVisualization::fillTaggedColumnCombos()
{
    for (int column = 0; column < static_cast<int>(dataset_->columnCount());
         ++column)
    {
        ColumnType columnType{dataset_->getColumnFormat(column)};
        if (columnType == ColumnType::NUMBER)
            ui->pricePerUnitCombo->addItem(dataset_->getHeaderName(column),
                                           QVariant(column));
        if (columnType == ColumnType::DATE)
            ui->dateCombo->addItem(dataset_->getHeaderName(column),
                                   QVariant(column));
    }
}

void DatasetVisualization::setAllItemsInColumnsListToState(Qt::CheckState state)
{
    const int topLevelItemsCount{ui->columnsList->topLevelItemCount()};
    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentItem{ui->columnsList->topLevelItem(i)};
        if (currentItem->flags().testFlag(Qt::ItemIsUserCheckable))
            currentItem->setCheckState(0, state);
    }
}

int DatasetVisualization::getCurrentValueFromCombo(QComboBox* combo)
{
    int column{Constants::NOT_SET_COLUMN};
    if (combo->currentIndex() != -1)
        column = combo->itemData(combo->currentIndex()).toInt();
    return column;
}

void DatasetVisualization::selectAllClicked()
{
    setAllItemsInColumnsListToState(Qt::Checked);
}

void DatasetVisualization::unselectAllClicked()
{
    setAllItemsInColumnsListToState(Qt::Unchecked);
}

void DatasetVisualization::refreshColumnList([[maybe_unused]] int newIndex)
{
    const int dateColumn{getCurrentValueFromCombo(ui->dateCombo)};
    const int priceColumn{getCurrentValueFromCombo(ui->pricePerUnitCombo)};

    int topLevelItemsCount{ui->columnsList->topLevelItemCount()};
    for (int i = 0; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentItem{ui->columnsList->topLevelItem(i)};
        int itemColumn{currentItem->data(0, Qt::UserRole).toInt()};
        if (priceColumn == itemColumn || dateColumn == itemColumn)
        {
            currentItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            currentItem->setData(0, Qt::CheckStateRole, QVariant());
        }
        else
        {
            currentItem->setFlags(Qt::ItemIsSelectable |
                                  Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            currentItem->setCheckState(0, Qt::Checked);
        }
    }
}
