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
    configurationFileContent_ = stream.readAll();
    QVERIFY(configurationFileContent_ != "");
}

void ConfigurationTest::testSavingConfigurationFile()
{
    Configuration::getInstance();
    QVERIFY(QFile::exists(configurationFileName_));
    QVERIFY(Configuration::getInstance().configValid());
    QVERIFY(Configuration::getInstance().save());
    QVERIFY(QFile::exists(configurationFileName_));
}

void ConfigurationTest::testReadingFilledConfigurationFile()
{
    QVERIFY(QFile::exists(configurationFileName_));
    QVERIFY(Configuration::getInstance().configValid());
    QCOMPARE(Configuration::getInstance().getStyleName(), defaultStyle_);
    QVERIFY(Configuration::getInstance().isUpdatePolicyPicked());
    QVERIFY(!Configuration::getInstance().needToCheckForUpdates());
}

void ConfigurationTest::testReadingEmptyConfigurationFile()
{
    QVERIFY(QFile::remove(configurationFileName_));
    QVERIFY(!QFile::exists(configurationFileName_));
    QVERIFY(!Configuration::getInstance().load());
    QVERIFY(!Configuration::getInstance().configValid());
    QCOMPARE(Configuration::getInstance().getStyleName(), defaultStyle_);
    QVERIFY(!Configuration::getInstance().isUpdatePolicyPicked());
    QVERIFY(!Configuration::getInstance().needToCheckForUpdates());
}

void ConfigurationTest::cleanupTestCase()
{
    QFile::remove(configurationFileName_);
    QFile file(configurationFileName_);
    QVERIFY(file.open(QIODevice::WriteOnly));
    QVERIFY(-1 != file.write(configurationFileContent_.toStdString().c_str()));
}
