#pragma once

#include <QObject>

/**
 * @brief Unit tests for Configuration class.
 */
class ConfigurationTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testSavingConfigurationFile();
    void testReadingFilledConfigurationFile();
    void testReadingEmptyConfigurationFile();
    void cleanupTestCase();

private:
    QString configurationFileContent_;
    QString configurationFileName_;
    const QString defaultStyle_{"Fusion"};
};
