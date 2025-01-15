#include "DatasetTest.h"

#include <QtTest/QtTest>

#include <Constants.h>

#include "DatasetDummy.h"

void DatasetTest::testGetColumnFormatColumnsSet()
{
    const unsigned int dateColumnIndex{0};
    const unsigned int valueColumnIndex{1};

    DatasetDummy dataset(QStringLiteral("a"));
    dataset.setTaggedColumn(ColumnTag::DATE, dateColumnIndex);
    dataset.setTaggedColumn(ColumnTag::VALUE, valueColumnIndex);

    auto [ok, column] = dataset.getTaggedColumn(ColumnTag::DATE);
    QCOMPARE(column, dateColumnIndex);
    std::tie(ok, column) = dataset.getTaggedColumn(ColumnTag::VALUE);
    QCOMPARE(column, valueColumnIndex);
}

void DatasetTest::testGetColumnFormatColumnsNotSet()
{
    const DatasetDummy dataset(QStringLiteral("a"));

    auto [ok, column] = dataset.getTaggedColumn(ColumnTag::DATE);
    QVERIFY(!ok);
    QCOMPARE(column, constants::NOT_SET_COLUMN);
    std::tie(ok, column) = dataset.getTaggedColumn(ColumnTag::VALUE);
    QVERIFY(!ok);
    QCOMPARE(column, constants::NOT_SET_COLUMN);
}
