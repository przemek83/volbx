#pragma once

#include <QObject>

class DatasetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testGetColumnFormatColumnsSet();

    void testGetColumnFormatColumnsNotSet();
};
