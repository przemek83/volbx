#include "DatasetVisualization.h"

#include <Common/Constants.h>
#include <Datasets/Dataset.h>

#include "ui_DatasetVisualization.h"

DatasetVisualization::DatasetVisualization(QWidget* parent)
    : QWidget(parent), ui_(std::make_unique<Ui::DatasetVisualization>())
{
    ui_->setupUi(this);

    ui_->columnsList->header()->setSectionsMovable(false);

    connect(ui_->searchLineEdit, &QLineEdit::textChanged, this,
            &DatasetVisualization::searchTextChanged);

    connect(ui_->columnsList, &QTreeWidget::currentItemChanged, this,
            &DatasetVisualization::currentColumnOnTreeChanged);

    connect(ui_->dateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DatasetVisualization::refreshColumnList);

    connect(ui_->pricePerUnitCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &DatasetVisualization::refreshColumnList);

    connect(ui_->SelectAll, &QPushButton::clicked, this,
            &DatasetVisualization::selectAllClicked);

    connect(ui_->UnselectAll, &QPushButton::clicked, this,
            &DatasetVisualization::unselectAllClicked);
}

DatasetVisualization::~DatasetVisualization() = default;

void DatasetVisualization::setDataset(std::unique_ptr<Dataset> dataset)
{
    clear();

    dataset_ = std::move(dataset);

    setupColumnsListWidget();

    fillTaggedColumnCombos();

    setTaggedColumns();

    ui_->taggedColumnsWidget->setEnabled(true);

    refreshColumnList(0);
}

void DatasetVisualization::clear()
{
    ui_->pricePerUnitCombo->clear();
    ui_->dateCombo->clear();
    ui_->columnsList->clear();
    ui_->columnsList->setEnabled(false);
    ui_->taggedColumnsWidget->setEnabled(false);
    dataset_ = nullptr;
}

void DatasetVisualization::searchTextChanged(const QString& newText)
{
    QTreeWidgetItemIterator it(ui_->columnsList);
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

    setTaggedColumnInDataset(ColumnTag::DATE, ui_->dateCombo);
    setTaggedColumnInDataset(ColumnTag::VALUE, ui_->pricePerUnitCombo);

    return std::move(dataset_);
}

void DatasetVisualization::currentColumnOnTreeChanged(
    const QTreeWidgetItem* current,
    [[maybe_unused]] const QTreeWidgetItem* previous)
{
    if (current == nullptr)
        return;

    emit currentColumnNeedSync(current->data(0, Qt::UserRole).toInt());
}

void DatasetVisualization::selectCurrentColumn(int column)
{
    if (QList<QTreeWidgetItem*> items{ui_->columnsList->selectedItems()};
        (!items.isEmpty()) &&
        (items.first()->data(0, Qt::UserRole).toInt() == column))
        return;

    const int itemCount{ui_->columnsList->topLevelItemCount()};
    for (int i{0}; i < itemCount; ++i)
    {
        QTreeWidgetItem* currentItem{ui_->columnsList->topLevelItem(i)};
        if (currentItem->data(0, Qt::UserRole).toInt() == column)
        {
            ui_->columnsList->setCurrentItem(currentItem);
            return;
        }
    }
}

void DatasetVisualization::setCurrentIndexUsingColumn(QComboBox* combo,
                                                      int column)
{
    const int itemCount{combo->count()};
    for (int i{0}; i < itemCount; ++i)
    {
        if (combo->itemData(i).toInt() == column)
        {
            combo->setCurrentIndex(i);
            return;
        }
    }
}

void DatasetVisualization::setupColumnsListWidget()
{
    ui_->columnsList->sortByColumn(constants::NOT_SET_COLUMN,
                                   Qt::AscendingOrder);
    ui_->columnsList->setSortingEnabled(false);

    const int count{dataset_->columnCount()};
    for (int column{0}; column < count; ++column)
    {
        const QStringList list{dataset_->getHeaderName(column),
                               getTypeDisplayNameForColumn(column)};
        auto* item{new QTreeWidgetItem(list)};
        item->setData(0, Qt::UserRole, QVariant(column));
        ui_->columnsList->addTopLevelItem(item);
    }

    ui_->columnsList->header()->resizeSections(QHeaderView::ResizeToContents);

    ui_->columnsList->setSortingEnabled(true);
    ui_->columnsList->sortByColumn(constants::NOT_SET_COLUMN,
                                   Qt::AscendingOrder);

    ui_->columnsList->setEnabled(true);
}

void DatasetVisualization::setTaggedColumns()
{
    ui_->dateCombo->blockSignals(true);
    ui_->pricePerUnitCombo->blockSignals(true);

    auto [dateOfTransactionPointed, dateColumn] =
        dataset_->getTaggedColumn(ColumnTag::DATE);
    if (dateOfTransactionPointed)
        setCurrentIndexUsingColumn(ui_->dateCombo, dateColumn);

    auto [pricePerUnitPointed, valueColumn] =
        dataset_->getTaggedColumn(ColumnTag::VALUE);
    if (pricePerUnitPointed)
        setCurrentIndexUsingColumn(ui_->pricePerUnitCombo, valueColumn);

    ui_->dateCombo->blockSignals(false);
    ui_->pricePerUnitCombo->blockSignals(false);
}

QVector<bool> DatasetVisualization::getActiveColumns() const
{
    const int topLevelItemsCount{ui_->columnsList->topLevelItemCount()};
    QVector<bool> activeColumns;
    activeColumns.resize(topLevelItemsCount);
    for (int i{0}; i < topLevelItemsCount; ++i)
    {
        const QTreeWidgetItem* currentItem{ui_->columnsList->topLevelItem(i)};
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
                                                    const QComboBox* combo)
{
    if (combo->currentIndex() == -1)
        return;

    const int column{combo->itemData(combo->currentIndex()).toInt()};
    dataset_->setTaggedColumn(tag, column);
}

QString DatasetVisualization::getTypeDisplayNameForColumn(int column) const
{
    switch (const ColumnType columnType{dataset_->getColumnFormat(column)};
            columnType)
    {
        case ColumnType::STRING:
            return typeNameString_;

        case ColumnType::NUMBER:
            return typeNameFloat_;

        case ColumnType::DATE:
            return typeNameDate_;

        default:
            Q_ASSERT(false);
    }

    return typeNameString_;
}

void DatasetVisualization::fillTaggedColumnCombos()
{
    const int count{dataset_->columnCount()};
    for (int column{0}; column < count; ++column)
    {
        const ColumnType columnType{dataset_->getColumnFormat(column)};
        if (columnType == ColumnType::NUMBER)
            ui_->pricePerUnitCombo->addItem(dataset_->getHeaderName(column),
                                            QVariant(column));
        if (columnType == ColumnType::DATE)
            ui_->dateCombo->addItem(dataset_->getHeaderName(column),
                                    QVariant(column));
    }
}

void DatasetVisualization::setAllItemsInColumnsListToState(
    Qt::CheckState state) const
{
    const int topLevelItemsCount{ui_->columnsList->topLevelItemCount()};
    for (int i{0}; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentItem{ui_->columnsList->topLevelItem(i)};
        if (currentItem->flags().testFlag(Qt::ItemIsUserCheckable))
            currentItem->setCheckState(0, state);
    }
}

int DatasetVisualization::getCurrentValueFromCombo(const QComboBox* combo)
{
    int column{constants::NOT_SET_COLUMN};
    if (combo->currentIndex() != -1)
        column = combo->itemData(combo->currentIndex()).toInt();
    return column;
}

void DatasetVisualization::selectAllClicked() const
{
    setAllItemsInColumnsListToState(Qt::Checked);
}

void DatasetVisualization::unselectAllClicked() const
{
    setAllItemsInColumnsListToState(Qt::Unchecked);
}

void DatasetVisualization::refreshColumnList(
    [[maybe_unused]] int newIndex) const
{
    const int dateColumn{getCurrentValueFromCombo(ui_->dateCombo)};
    const int priceColumn{getCurrentValueFromCombo(ui_->pricePerUnitCombo)};

    const int topLevelItemsCount{ui_->columnsList->topLevelItemCount()};
    for (int i{0}; i < topLevelItemsCount; ++i)
    {
        QTreeWidgetItem* currentItem{ui_->columnsList->topLevelItem(i)};
        const int itemColumn{currentItem->data(0, Qt::UserRole).toInt()};
        if ((priceColumn == itemColumn) || (dateColumn == itemColumn))
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
