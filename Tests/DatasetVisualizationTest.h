#pragma once

#include <QObject>
#include <memory>

#include <Dataset.h>

class DatasetVisualizationTest : public QObject
{
    Q_OBJECT

private:
    std::unique_ptr<Dataset> getDataset() const;

private slots:
    void testGetDataset() const;

    void testGetDatasetWithoutSettingIt() const;

    void testClearDataset() const;
};
