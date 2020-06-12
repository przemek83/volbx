#ifndef PLOTDATAPROVIDERTEST_H
#define PLOTDATAPROVIDERTEST_H

#include <QObject>
#include <QSignalSpy>

class PlotDataProviderTest : public QObject
{
public:
    explicit PlotDataProviderTest(QObject* parent = nullptr);

    Q_OBJECT
private Q_SLOTS:
    void testNotSetGroupingColumn();
    void testRecomputeGroupingDataNotSetColumn();

private:
    static constexpr int NO_SIGNAL{0};
};

#endif  // PLOTDATAPROVIDERTEST_H
