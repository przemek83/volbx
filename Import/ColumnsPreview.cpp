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

    const unsigned int columns{dataset->columnCount()};
    setColumnCount(columns);

    QStringList labels;
    labels.reserve(columns);
    for (unsigned int i = 0; i < columns; ++i)
        labels.append(dataset->getHeaderName(i));

    setHorizontalHeaderLabels(labels);

    QVector<QVector<QVariant> > sampleData{dataset->retrieveSampleData()};

    const int rows{sampleData.size()};
    setRowCount(rows);

    Q_ASSERT(sampleData.isEmpty() ? true
                                  : columns == static_cast<unsigned int>(
                                                   sampleData.at(0).size()));

    for (int i = 0; i < rows; ++i)
    {
        for (unsigned int j = 0; j < columns; ++j)
        {
            QTableWidgetItem* item =
                new QTableWidgetItem(sampleData.at(i).at(j).toString());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            setItem(i, j, item);
        }
    }
}

void ColumnsPreview::clear()
{
    QTableWidget::clear();
    setColumnCount(0);
    setRowCount(0);
}

void ColumnsPreview::selectCurrentColumn(int column)
{
    const QTableWidgetItem* itemToSelect{item(0, column)};
    if (itemToSelect != nullptr && !itemToSelect->isSelected())
        selectColumn(column);
}

void ColumnsPreview::onItemSelectionChanged()
{
    const QList<QTableWidgetItem*> selectedItemsList{selectedItems()};
    if (selectedItemsList.isEmpty())
        return;

    const QTableWidgetItem* selectedItem{selectedItems().first()};
    if (selectedItem != nullptr)
        Q_EMIT currentColumnNeedSync(selectedItem->column());
}
