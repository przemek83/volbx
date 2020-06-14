#ifndef FILTERINGPROXYMODELTEST_H
#define FILTERINGPROXYMODELTEST_H

#include <QObject>

class QStandardItem;
class FilteringProxyModel;

class FilteringProxyModelTest : public QObject
{
    Q_OBJECT
public:
    explicit FilteringProxyModelTest(QObject* parent = nullptr);

private Q_SLOTS:
    void testNoFilter();

    void testStringFilter();

    void testDateFilter();

    void testNumberFilter();

private:
    void checkProxyHasAllItems(const FilteringProxyModel& proxy,
                               const QList<QStandardItem*>& items);

    QList<QStandardItem*> getStringItems();
    QList<QStandardItem*> getDateItems();
    QList<QStandardItem*> getNumberItems();
};

#endif  // FILTERINGPROXYMODELTEST_H
