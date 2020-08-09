#pragma once

#include <QObject>

/**
 * @brief Unit tests for Configuration class.
 */
class ConfigurationTest : public QObject
{
    Q_OBJECT
private:
    QString configurationFile_;

    QString configurationFileName_;

private slots:
    void initTestCase();
    void testSavingConfigurationFile();
    void testReadingFilledConfigurationFile();
    void testReadingEmptyConfigurationFile();
    void cleanupTestCase();
};
