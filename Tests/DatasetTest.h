#pragma once

#include <QObject>

class DatasetTest : public QObject
{
    Q_OBJECT
private slots:
    void testGetColumnFormatColumnsSet();

    void testGetColumnFormatColumnsNotSet();
};
