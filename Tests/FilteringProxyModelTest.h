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
    void testNoFilter();

    void testStringFilter();

    void testDateFilter();
    void testDateFilterFilterEmptyDates();

    void testNumberFilter();

private:
    static void checkProxyHasAllItems(const FilteringProxyModel& proxy,
                                      const QList<QStandardItem*>& items);

    static QVariant getData(QStandardItem* item);
    static QStandardItem* createItem(const QVariant& data);
    static QList<QStandardItem*> getStringItems();
    static QList<QStandardItem*> getDateItems();
    static QList<QStandardItem*> getNumberItems();
};
