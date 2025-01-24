#pragma once

#include <QObject>

class QStandardItem;
class FilteringProxyModel;

class FilteringProxyModelTest : public QObject
{
    Q_OBJECT
private slots:
    static void testNoFilter();

    static void testStringFilter();

    static void testDateFilter();
    static void testDateFilterFilterEmptyDates();

    static void testNumberFilter();

private:
    static void checkProxyHasAllItems(const FilteringProxyModel& proxy,
                                      const QList<QStandardItem*>& items);

    static QVariant getData(QStandardItem* item);
    static QStandardItem* createItem(const QVariant& data);
    static QList<QStandardItem*> getStringItems();
    static QList<QStandardItem*> getDateItems();
    static QList<QStandardItem*> getNumberItems();
};
