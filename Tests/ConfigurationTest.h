#pragma once

#include <QObject>

class ConfigurationTest : public QObject
{
    Q_OBJECT
private slots:
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
