#pragma once

#include <QObject>

class TestConstants : public QObject
{
    Q_OBJECT
private slots:
    static void testGetUpdaterName();
    static void testGetExeFileSuffix();
    static void testGetTemporaryFileSuffix();
    static void testGetConfigurationFileName();
    static void testGetProgressBarTitle();
    static void testGetProgressBarFullCounter();
};