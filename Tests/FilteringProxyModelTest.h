#ifndef FILTERINGPROXYMODELTEST_H
#define FILTERINGPROXYMODELTEST_H

#include <QObject>

class FilteringProxyModelTest : public QObject
{
    Q_OBJECT
public:
    explicit FilteringProxyModelTest(QObject* parent = nullptr);

private Q_SLOTS:
    void testStringFilterNoFilter();
    void testStringFilter();
};

#endif  // FILTERINGPROXYMODELTEST_H
