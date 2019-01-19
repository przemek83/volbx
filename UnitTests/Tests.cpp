#include <QtTest/QtTest>

#include "ConfigurationTest.h"
#include "ConstantsTest.h"
#include "SpreadsheetsTest.h"
#include "InnerTests.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConfigurationTest configurationTest;
    QTest::qExec(&configurationTest);

    ConstantsTest constantsTest;
    QTest::qExec(&constantsTest);

    SpreadsheetsTest spreadsheetsTest;
    QTest::qExec(&spreadsheetsTest);

    InnerTests ineerTests;
    QTest::qExec(&ineerTests);

    return 0;
}
