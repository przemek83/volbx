#include <QtTest/QtTest>

#include "ConfigurationTest.h"
#include "DatasetTest.h"
#include "DetailedSpreadsheetsTest.h"
#include "FilteringProxyModelTest.h"
#include "InnerTests.h"
#include "PlotDataProviderTest.h"
#include "SpreadsheetsTest.h"

int main(int argc, char* argv[])
{
    QLocale::setDefault(QLocale::c());

    const QApplication a(argc, argv);

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

    InnerTests innerTests;
    QTest::qExec(&innerTests);

    DatasetTest datasetTest;
    QTest::qExec(&datasetTest);

    return 0;
}
