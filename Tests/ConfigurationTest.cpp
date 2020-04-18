#include "ConfigurationTest.h"

#include <QtTest/QtTest>

#include "Common/Configuration.h"
#include "Common/Constants.h"

void ConfigurationTest::initTestCase()
{
    configurationFileName_ = QCoreApplication::applicationDirPath() + "/" +
                             Constants::getConfigurationFileName();

    QFile file(configurationFileName_);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QTextStream stream(&file);

    configurationFile_ = stream.readAll();

    QVERIFY(configurationFile_ != "");

    file.close();
}

void ConfigurationTest::testSavingConfigurationFile()
{
    Configuration::getInstance();
    QVERIFY(QFile::exists(configurationFileName_));
    QVERIFY(Configuration::getInstance().configWasValid());
    QVERIFY(Configuration::getInstance().save());
    QVERIFY(QFile::exists(configurationFileName_));
}

void ConfigurationTest::testReadingFilledConfigurationFile()
{
    QVERIFY(QFile::exists(configurationFileName_));
    QVERIFY(Configuration::getInstance().configWasValid());
    QCOMPARE(Configuration::getInstance().getStyle(), QString("Fusion"));
    QVERIFY(!Configuration::getInstance().needToShowUpdatePickerDialog());
    QVERIFY(!Configuration::getInstance().needToCheckForUpdates());
}

void ConfigurationTest::testReadingEmptyConfigurationFile()
{
    QVERIFY(QFile::remove(configurationFileName_));
    QVERIFY(!QFile::exists(configurationFileName_));
    QVERIFY(!Configuration::getInstance().load());
    QVERIFY(!Configuration::getInstance().configWasValid());
    QCOMPARE(Configuration::getInstance().getStyle(), QString("Fusion"));
    QVERIFY(!Configuration::getInstance().needToShowUpdatePickerDialog());
    QVERIFY(!Configuration::getInstance().needToCheckForUpdates());
}

void ConfigurationTest::cleanupTestCase()
{
    QFile::remove(configurationFileName_);
    QFile file(configurationFileName_);

    QVERIFY(file.open(QIODevice::WriteOnly));

    QVERIFY(-1 != file.write(configurationFile_.toStdString().c_str()));
}
