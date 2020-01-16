#include "ConstantsTest.h"

#include <QtTest/QtTest>

#include "Common/Constants.h"

void ConstantsTest::testDatesConversion()
{
    QCOMPARE(Constants::stringFromDays(0), QString("1/1/1970"));
    QCOMPARE(Constants::stringFromDays(-100), QString("23/9/1969"));
    QCOMPARE(Constants::stringFromDays(20000), QString("4/10/2024"));
}
