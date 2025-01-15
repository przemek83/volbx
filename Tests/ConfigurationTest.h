#pragma once

#include <QObject>

/**
 * @brief Unit tests for Configuration class.
 */
class ConfigurationTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testSavingConfigurationFile() const;
    void testReadingFilledConfigurationFile() const;
    void testReadingEmptyConfigurationFile() const;
    void cleanupTestCase() const;

private:
    QString configurationFileContent_;
    QString configurationFileName_;
    const QString defaultStyle_{QStringLiteral("Fusion")};
};
