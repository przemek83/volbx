#include "ConstantsTest.h"

#include <QTest>

#include "Constants.h"

void TestConstants::testGetUpdaterName()
{
    QCOMPARE(constants::getUpdaterName(), QStringLiteral("VersionCheck"));
}

void TestConstants::testGetExeFileSuffix()
{
    QCOMPARE(constants::getExeFileSuffix(), QStringLiteral(".exe"));
}

void TestConstants::testGetTemporaryFileSuffix()
{
    QCOMPARE(constants::getTemporaryFileSuffix(), QStringLiteral(".tmp"));
}

void TestConstants::testGetConfigurationFileName()
{
    QCOMPARE(constants::getConfigurationFileName(), QStringLiteral("config"));
}

void TestConstants::testGetProgressBarTitle()
{
    QCOMPARE(constants::getProgressBarTitle(constants::BarTitle::LOADING),
             QObject::tr("Loading"));
    QCOMPARE(constants::getProgressBarTitle(constants::BarTitle::SAVING),
             QObject::tr("Saving"));
    QCOMPARE(constants::getProgressBarTitle(constants::BarTitle::ANALYSING),
             QObject::tr("Analysing"));
}

void TestConstants::testGetProgressBarFullCounter()
{
    QCOMPARE(constants::getProgressBarFullCounter(), 100);
}
