#ifndef PLOTDATAPROVIDERTEST_H
#define PLOTDATAPROVIDERTEST_H

#include <QObject>

class PlotDataProviderTest : public QObject
{
public:
    explicit PlotDataProviderTest(QObject* parent = nullptr);

    Q_OBJECT
private Q_SLOTS:
    void testNotSetGroupingColumn();
};

#endif  // PLOTDATAPROVIDERTEST_H
