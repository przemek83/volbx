#pragma once

#include <memory>

#include "ImportTab.h"

class Dataset;

/**
 *@brief Inner dataset type import tab.
 */
class DatasetImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit DatasetImportTab(QWidget* parent = nullptr);

    ~DatasetImportTab() override = default;

    std::unique_ptr<Dataset> getDataset() override;

    bool datasetsAreAvailable();

private:
    void clear();

    void createDataset(const QString& datasetName);

    void setDataset(std::unique_ptr<Dataset> dataset);

private Q_SLOTS:
    void selectedDatasetChanged(const QString& current);
};
