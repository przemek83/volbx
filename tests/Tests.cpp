#include <QTest>

#include "ConfigurationTest.h"
#include "ConstantsTest.h"
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
    status = std::max(status, QTest::qExec(&configurationTest));

    SpreadsheetsTest spreadsheetsTest;
    status = std::max(status, QTest::qExec(&spreadsheetsTest));

    DetailedSpreadsheetsTest detailedSpreadsheetsTest;
    status = std::max(status, QTest::qExec(&detailedSpreadsheetsTest));

    PlotDataProviderTest plotDataProviderTest;
    status = std::max(status, QTest::qExec(&plotDataProviderTest));

    FilteringProxyModelTest filteringProxyModelTest;
    status = std::max(status, QTest::qExec(&filteringProxyModelTest));

    InnerTests innerTests;
    status = std::max(status, QTest::qExec(&innerTests));

    DatasetTest datasetTest;
    status = std::max(status, QTest::qExec(&datasetTest));

    DatasetVisualizationTest datasetVisualizationTest;
    status = std::max(status, QTest::qExec(&datasetVisualizationTest));

    TestConstants testConstants;
    status = std::max(status, QTest::qExec(&testConstants));

    return status;
}
