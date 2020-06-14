#include "FilteringProxyModelTest.h"

#include <QDate>
#include <QStandardItemModel>
#include <QtTest/QtTest>

#include <FilteringProxyModel.h>

FilteringProxyModelTest::FilteringProxyModelTest(QObject* parent)
    : QObject(parent)
{
}

void FilteringProxyModelTest::testNoFilter()
{
    QList<QStandardItem*> items{getStringItems()};
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn(items);
    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    checkProxyHasAllItems(proxy, items);
}

void FilteringProxyModelTest::testStringFilter()
{
    QList<QStandardItem*> items{getStringItems()};
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
    QList<QStandardItem*> items{getDateItems()};
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn(items);

    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    proxy.setDateFilter(0, QDate(2020, 3, 11), QDate(2020, 4, 27), false);

    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(proxy.data(proxy.index(0, 0)), items[0]->data(Qt::DisplayRole));
    QCOMPARE(proxy.data(proxy.index(1, 0)), items[1]->data(Qt::DisplayRole));
}

void FilteringProxyModelTest::testNumberFilter()
{
    QList<QStandardItem*> items{getNumberItems()};
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn(items);

    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    proxy.setNumericFilter(0, 2, 10);

    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(proxy.data(proxy.index(0, 0)), items[1]->data(Qt::DisplayRole));
    QCOMPARE(proxy.data(proxy.index(1, 0)), items[2]->data(Qt::DisplayRole));
}

void FilteringProxyModelTest::checkProxyHasAllItems(
    const FilteringProxyModel& proxy, const QList<QStandardItem*>& items)
{
    QCOMPARE(proxy.rowCount(), items.size());
    for (int i = 0; i < items.size(); ++i)
        QCOMPARE(proxy.data(proxy.index(i, 0)), items[i]->text());
}

QList<QStandardItem*> FilteringProxyModelTest::getStringItems()
{
    return {new QStandardItem("a"), new QStandardItem("b"),
            new QStandardItem("c")};
}

QList<QStandardItem*> FilteringProxyModelTest::getDateItems()
{
    QList<QStandardItem*> dateItems;
    QStandardItem* dateItem = new QStandardItem();
    dateItem->setData(QDate(2020, 4, 20), Qt::DisplayRole);
    dateItems.append(dateItem);
    dateItem = new QStandardItem();
    dateItem->setData(QDate(2020, 4, 25), Qt::DisplayRole);
    dateItems.append(dateItem);
    dateItem = new QStandardItem();
    dateItem->setData(QDate(2020, 4, 28), Qt::DisplayRole);
    dateItems.append(dateItem);
    return dateItems;
}

QList<QStandardItem*> FilteringProxyModelTest::getNumberItems()
{
    QList<QStandardItem*> dateItems;
    QStandardItem* dateItem = new QStandardItem();
    dateItem->setData(1., Qt::DisplayRole);
    dateItems.append(dateItem);
    dateItem = new QStandardItem();
    dateItem->setData(2., Qt::DisplayRole);
    dateItems.append(dateItem);
    dateItem = new QStandardItem();
    dateItem->setData(3., Qt::DisplayRole);
    dateItems.append(dateItem);
    return dateItems;
}
