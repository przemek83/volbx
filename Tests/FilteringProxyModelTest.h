#pragma once

#include <QObject>

class QStandardItem;
class FilteringProxyModel;

/**
 * @brief Tests for FilteringProxyModel class.
 */
class FilteringProxyModelTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    static void testNoFilter();

    static void testStringFilter();

    static void testDateFilter();
    void testDateFilterFilterEmptyDates();

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
