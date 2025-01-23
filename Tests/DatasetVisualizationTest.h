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
    void testGetDataset() const;

    static void testGetDatasetWithoutSettingIt();

    void testClearDataset() const;

    void testSelectCurrentColumn() const;
};
