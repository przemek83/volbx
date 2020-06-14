#include "FilteringProxyModelTest.h"

#include <QDate>
#include <QStandardItemModel>
#include <QtTest/QtTest>

#include <FilteringProxyModel.h>

FilteringProxyModelTest::FilteringProxyModelTest(QObject* parent)
    : QObject(parent)
{
}

void FilteringProxyModelTest::testStringFilterNoFilter()
{
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn({new QStandardItem("a"),
                                    new QStandardItem("b"),
                                    new QStandardItem("c")});
    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);

    QCOMPARE(proxy.rowCount(), 3);
    QCOMPARE(proxy.data(proxy.index(0, 0)), "a");
    QCOMPARE(proxy.data(proxy.index(1, 0)), "b");
    QCOMPARE(proxy.data(proxy.index(2, 0)), "c");
}

void FilteringProxyModelTest::testStringFilter()
{
    QStandardItemModel standardItemModel;
    standardItemModel.appendColumn({new QStandardItem("a"),
                                    new QStandardItem("b"),
                                    new QStandardItem("c")});
    FilteringProxyModel proxy;
    proxy.setSourceModel(&standardItemModel);
    proxy.setStringFilter(0, {"b"});

    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(proxy.data(proxy.index(0, 0)), "a");
    QCOMPARE(proxy.data(proxy.index(1, 0)), "c");
}
