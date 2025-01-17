#pragma once

#include <QObject>

class DatasetVisualizationTest : public QObject
{
    Q_OBJECT
private slots:
    void testGetDataset() const;

    void testGetDatasetWithoutSettingIt() const;
};
