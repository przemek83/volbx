#include "ColumnsPreview.h"

#include <QTableWidgetItem>

#include "Datasets/Dataset.h"

ColumnsPreview::ColumnsPreview(QWidget* parent) : QTableWidget(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectColumns);

    connect(this, &QTableWidget::itemSelectionChanged, this,
            &ColumnsPreview::onItemSelectionChanged);
}

void ColumnsPreview::setDatasetSampleInfo(
    const std::unique_ptr<Dataset>& dataset)
{
    clear();

    const int columns{dataset->columnCount()};
    setColumnCount(columns);

    setLabels(dataset);

    const QVector<QVector<QVariant>> sampleData{dataset->retrieveSampleData()};
    const int rows{static_cast<int>(sampleData.size())};
    setRowCount(rows);
    for (int i{0}; i < rows; ++i)
        for (int j{0}; j < columns; ++j)
            setItem(i, j, createItem(sampleData.at(i).at(j).toString()));
}

void ColumnsPreview::clear()
{
    QTableWidget::clear();
    setColumnCount(0);
    setRowCount(0);
}

QTableWidgetItem* ColumnsPreview::createItem(const QString& name)
{
    QTableWidgetItem* item{new QTableWidgetItem(name)};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

void ColumnsPreview::setLabels(const std::unique_ptr<Dataset>& dataset)
{
    const int columns{dataset->columnCount()};
    QStringList labels;
    labels.reserve(columns);
    for (int i = 0; i < columns; ++i)
        labels.append(dataset->getHeaderName(i));

    setHorizontalHeaderLabels(labels);
}

void ColumnsPreview::selectCurrentColumn(int column)
{
    const QTableWidgetItem* itemToSelect{item(0, column)};
    if ((itemToSelect != nullptr) && (!itemToSelect->isSelected()))
        selectColumn(column);
}

void ColumnsPreview::onItemSelectionChanged()
{
    const QList<QTableWidgetItem*> selectedItemsList{selectedItems()};
    if (!selectedItemsList.isEmpty())
        emit currentColumnNeedSync(selectedItemsList.first()->column());
}
