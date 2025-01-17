#include <QtTest/QtTest>

#include "ConfigurationTest.h"
#include "DatasetTest.h"
#include "DatasetVisualizationTest.h"
#include "DetailedSpreadsheetsTest.h"
#include "FilteringProxyModelTest.h"
#include "InnerTests.h"
#include "PlotDataProviderTest.h"
#include "SpreadsheetsTest.h"

int main(int argc, char* argv[])
{
    QLocale::setDefault(QLocale::c());

    const QApplication a(argc, argv);

    int status{EXIT_SUCCESS};
    ConfigurationTest configurationTest;
    status |= QTest::qExec(&configurationTest);

    SpreadsheetsTest spreadsheetsTest;
    status |= QTest::qExec(&spreadsheetsTest);

    DetailedSpreadsheetsTest detailedSpreadsheetsTest;
    status |= QTest::qExec(&detailedSpreadsheetsTest);

    PlotDataProviderTest plotDataProviderTest;
    status |= QTest::qExec(&plotDataProviderTest);

    FilteringProxyModelTest filteringProxyModelTest;
    status |= QTest::qExec(&filteringProxyModelTest);

    InnerTests innerTests;
    status |= QTest::qExec(&innerTests);

    DatasetTest datasetTest;
    status |= QTest::qExec(&datasetTest);

    DatasetVisualizationTest datasetVisualizationTest;
    status |= QTest::qExec(&datasetVisualizationTest);

    return status;
}
