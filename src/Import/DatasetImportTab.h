#pragma once

#include "ImportTab.h"

class Dataset;

class DatasetImportTab : public ImportTab
{
    Q_OBJECT
public:
    DatasetImportTab();

    bool datasetsAreAvailable() const;

private:
    void clear();

    void createDataset(const QString& datasetName);

private slots:
    void selectedDatasetChanged(const QString& current);
};
