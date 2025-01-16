#pragma once

#include <QObject>

class DatasetTest : public QObject
{
    Q_OBJECT
private slots:
    static void testGetColumnFormatColumnsSet();

    static void testGetColumnFormatColumnsNotSet();
};
