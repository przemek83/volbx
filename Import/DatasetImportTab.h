#ifndef DATASETIMPORTTAB_H
#define DATASETIMPORTTAB_H

#include "ImportTab.h"

/**
 *@brief Inner dataset type import tab.
 */
class DatasetImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit DatasetImportTab(QWidget* parent = nullptr);

    ~DatasetImportTab() override = default;

    DatasetImportTab& operator=(const DatasetImportTab& other) = delete;
    DatasetImportTab(const DatasetImportTab& other) = delete;

    DatasetImportTab& operator=(DatasetImportTab&& other) = delete;
    DatasetImportTab(DatasetImportTab&& other) = delete;

    DatasetDefinition* getDatasetDefinition() override;

    bool datasetsAreAvailable();

private Q_SLOTS:
    void selectedDatasetChanged(const QString& current);
};

#endif // DATASETIMPORTTAB_H
