#pragma once

#include "ImportTab.h"

class Dataset;

class DatasetImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit DatasetImportTab(QWidget* parent);

    bool datasetsAreAvailable() const;

private:
    void clear();

    void createDataset(const QString& datasetName);

private slots:
    void selectedDatasetChanged(const QString& current);
};
