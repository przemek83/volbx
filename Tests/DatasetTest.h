#pragma once

#include <QObject>

class DatasetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    static void testGetColumnFormatColumnsSet();

    static void testGetColumnFormatColumnsNotSet();
};
