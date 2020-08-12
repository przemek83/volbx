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
    void checkProxyHasAllItems(const FilteringProxyModel& proxy,
                               const QList<QStandardItem*>& items);

    QVariant getData(QStandardItem* item) const;
    QStandardItem* createItem(QVariant data) const;
    QList<QStandardItem*> getStringItems();
    QList<QStandardItem*> getDateItems();
    QList<QStandardItem*> getNumberItems();
};
