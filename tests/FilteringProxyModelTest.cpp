#include "FilteringProxyModelTest.h"

#include <QDate>
#include <QStandardItemModel>
#include <QTest>

#include <FilteringProxyModel.h>

void FilteringProxyModelTest::testNoFilter()
{
    const QList<QStandardItem*> items{getStringItems()};
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn(items);
    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    checkProxyHasAllItems(proxy, items);
}

void FilteringProxyModelTest::testStringFilter()
{
    const QList<QStandardItem*> items{getStringItems()};
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn(items);
    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    proxy.setStringFilter(0, {items[1]->text()});

    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(proxy.data(proxy.index(0, 0)), items[0]->text());
    QCOMPARE(proxy.data(proxy.index(1, 0)), items[2]->text());
}

void FilteringProxyModelTest::testDateFilter()
{
    const QList<QStandardItem*> items{getDateItems()};
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn(items);

    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    proxy.setDateFilter(0, QDate(2020, 3, 11), QDate(2020, 4, 27), false);

    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(proxy.data(proxy.index(0, 0)), getData(items[0]));
    QCOMPARE(proxy.data(proxy.index(1, 0)), getData(items[1]));
}

void FilteringProxyModelTest::testDateFilterFilterEmptyDates()
{
    QList<QStandardItem*> items{getDateItems()};
    items.append(new QStandardItem());
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn(items);

    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    proxy.setDateFilter(0, QDate(2020, 3, 11), QDate(2020, 4, 27), true);
    QCOMPARE(proxy.rowCount(), 2);

    proxy.setDateFilter(0, QDate(2020, 3, 11), QDate(2020, 4, 27), false);
    QCOMPARE(proxy.rowCount(), 3);
}

void FilteringProxyModelTest::testNumberFilter()
{
    const QList<QStandardItem*> items{getNumberItems()};
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn(items);

    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    proxy.setNumericFilter(0, 2, 10);

    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(proxy.data(proxy.index(0, 0)), getData(items[1]));
    QCOMPARE(proxy.data(proxy.index(1, 0)), getData(items[2]));
}

void FilteringProxyModelTest::checkProxyHasAllItems(
    const FilteringProxyModel& proxy, const QList<QStandardItem*>& items)
{
    QCOMPARE(proxy.rowCount(), items.size());
    for (int i{0}; i < items.size(); ++i)
        QCOMPARE(proxy.data(proxy.index(i, 0)), items[i]->text());
}

QVariant FilteringProxyModelTest::getData(const QStandardItem* item)
{
    return item->data(Qt::DisplayRole);
}

QList<QStandardItem*> FilteringProxyModelTest::getStringItems()
{
    return {new QStandardItem(QStringLiteral("a")),
            new QStandardItem(QStringLiteral("b")),
            new QStandardItem(QStringLiteral("c"))};
}

QStandardItem* FilteringProxyModelTest::createItem(const QVariant& data)
{
    auto* dateItem{new QStandardItem()};
    dateItem->setData(data, Qt::DisplayRole);
    return dateItem;
}

QList<QStandardItem*> FilteringProxyModelTest::getDateItems()
{
    QList<QStandardItem*> dateItems;
    dateItems.append(createItem(QDate(2020, 4, 20)));
    dateItems.append(createItem(QDate(2020, 4, 25)));
    dateItems.append(createItem(QDate(2020, 4, 28)));
    return dateItems;
}

QList<QStandardItem*> FilteringProxyModelTest::getNumberItems()
{
    QList<QStandardItem*> dateItems;
    dateItems.append(createItem(1.));
    dateItems.append(createItem(2.));
    dateItems.append(createItem(3.));
    return dateItems;
}
