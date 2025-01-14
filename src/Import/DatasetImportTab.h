#pragma once

#include <memory>

#include "ImportTab.h"

class Dataset;

class DatasetImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit DatasetImportTab(QWidget* parent = nullptr);

    bool datasetsAreAvailable();

private:
    void clear();

    void createDataset(const QString& datasetName);

private Q_SLOTS:
    void selectedDatasetChanged(const QString& current);
};
