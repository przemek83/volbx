#include "ConfigurationTest.h"

#include <QtTest/QtTest>

#include "Common/Configuration.h"
#include "Common/Constants.h"

void ConfigurationTest::initTestCase()
{
    configurationFileName_ = QCoreApplication::applicationDirPath() + "/" +
                             QString(Constants::configurationFile);

    QFile file(configurationFileName_);
    QVERIFY(true == file.open(QIODevice::ReadOnly));
    QTextStream stream(&file);

    configurationFile_ = stream.readAll();

    QVERIFY(configurationFile_ != "");

    file.close();
}

void ConfigurationTest::testSavingConfigurationFile()
{
    Configuration::getInstance();
    QVERIFY(true == QFile::exists(configurationFileName_));
    QVERIFY(true == Configuration::getInstance().configWasValid());
    QVERIFY(true == Configuration::getInstance().save());
    QVERIFY(true == QFile::exists(configurationFileName_));
}

void ConfigurationTest::testReadingFilledConfigurationFile()
{
    QVERIFY(true == QFile::exists(configurationFileName_));
    QVERIFY(true == Configuration::getInstance().configWasValid());
    QCOMPARE(Configuration::getInstance().getStyle(), QString("Fusion"));
    QVERIFY(false == Configuration::getInstance().needToShowUpdatePickerDialog());
    QVERIFY(false == Configuration::getInstance().needToCheckForUpdates());
}

void ConfigurationTest::testReadingEmptyConfigurationFile()
{
    QVERIFY(true == QFile::remove(configurationFileName_));
    QVERIFY(false == QFile::exists(configurationFileName_));
    QVERIFY(false == Configuration::getInstance().load());
    QVERIFY(false == Configuration::getInstance().configWasValid());
    QCOMPARE(Configuration::getInstance().getStyle(), QString("Fusion"));
    QVERIFY(false == Configuration::getInstance().needToShowUpdatePickerDialog());
    QVERIFY(false == Configuration::getInstance().needToCheckForUpdates());
}

void ConfigurationTest::cleanupTestCase()
{
    QFile::remove(configurationFileName_);
    QFile file(configurationFileName_);

    QVERIFY(true == file.open(QIODevice::WriteOnly));

    QVERIFY(-1 != file.write(configurationFile_.toStdString().c_str()));
}
