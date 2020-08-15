#include <QtTest/QtTest>

#include "ConfigurationTest.h"
#include "DetailedSpreadsheetsTest.h"
#include "FilteringProxyModelTest.h"
#include "InnerTests.h"
#include "PlotDataProviderTest.h"
#include "SpreadsheetsTest.h"

int main(int argc, char* argv[])
{
    QLocale::setDefault(QLocale::c());

    QApplication a(argc, argv);

    ConfigurationTest configurationTest;
    QTest::qExec(&configurationTest);

    SpreadsheetsTest spreadsheetsTest;
    QTest::qExec(&spreadsheetsTest);

    DetailedSpreadsheetsTest detailedSpreadsheetsTest;
    QTest::qExec(&detailedSpreadsheetsTest);

    PlotDataProviderTest plotDataProviderTest;
    QTest::qExec(&plotDataProviderTest);

    FilteringProxyModelTest filteringProxyModelTest;
    QTest::qExec(&filteringProxyModelTest);

    InnerTests ineerTests;
    QTest::qExec(&ineerTests);

    return 0;
}
