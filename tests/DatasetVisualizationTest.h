#pragma once

#include <QObject>
#include <memory>

#include <Dataset.h>

class DatasetVisualizationTest : public QObject
{
    Q_OBJECT

private:
    static std::unique_ptr<Dataset> getDataset();

private slots:
    static void testGetDataset();

    static void testGetDatasetWithoutSettingIt();

    static void testClearDataset();

    static void testSelectCurrentColumn();
};
