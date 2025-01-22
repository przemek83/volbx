#include "ColumnsPreview.h"

#include <QTableWidgetItem>

ColumnsPreview::ColumnsPreview(QWidget* parent) : QTableWidget(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectColumns);

    connect(this, &QTableWidget::itemSelectionChanged, this,
            &ColumnsPreview::onItemSelectionChanged);
}

void ColumnsPreview::setDatasetSampleInfo(
    int columnsCount, const QVector<QVector<QVariant>>& sampleData)
{
    purge();

    const int columns{columnsCount};
    setColumnCount(columns);

    const int rows{static_cast<int>(sampleData.size())};
    setRowCount(rows);
    for (int i{0}; i < rows; ++i)
        for (int j{0}; j < columns; ++j)
            setItem(i, j, createItem(sampleData.at(i).at(j).toString()));
}

void ColumnsPreview::purge()
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
