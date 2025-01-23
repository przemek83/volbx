#include "ConfigurationTest.h"

#include <QTest>

#include "Common/Configuration.h"
#include "Common/Constants.h"

void ConfigurationTest::initTestCase()
{
    configurationFileName_ = QCoreApplication::applicationDirPath() + "/" +
                             constants::getConfigurationFileName();

    QFile file(configurationFileName_);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QTextStream stream(&file);
    configurationFileContent_ = stream.readAll();
    QVERIFY(!configurationFileContent_.isEmpty());
}

void ConfigurationTest::testSavingConfigurationFile() const
{
    Configuration config;
    QVERIFY(QFile::exists(configurationFileName_));
    QVERIFY(config.configValid());
    QVERIFY(config.save());
    QVERIFY(QFile::exists(configurationFileName_));
}

void ConfigurationTest::testReadingFilledConfigurationFile() const
{
    Configuration config;
    QVERIFY(QFile::exists(configurationFileName_));
    QVERIFY(config.configValid());
    QCOMPARE(config.getStyleName(), defaultStyle_);
    QVERIFY(config.isUpdatePolicyPicked());
    QVERIFY(!config.needToCheckForUpdates());
}

void ConfigurationTest::testReadingEmptyConfigurationFile() const
{
    QVERIFY(QFile::remove(configurationFileName_));
    QVERIFY(!QFile::exists(configurationFileName_));

    Configuration config;
    QVERIFY(!config.configValid());
    QCOMPARE(config.getStyleName(), defaultStyle_);
    QVERIFY(!config.isUpdatePolicyPicked());
    QVERIFY(!config.needToCheckForUpdates());
}

void ConfigurationTest::cleanupTestCase() const
{
    QFile file(configurationFileName_);
    QVERIFY(file.open(QIODeviceBase::Truncate | QIODevice::WriteOnly));
    QVERIFY(-1 != file.write(configurationFileContent_.toStdString().c_str()));
}
