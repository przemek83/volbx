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
    explicit DatasetImportTab(QWidget *parent = nullptr);

    ~DatasetImportTab() override = default;

    DatasetDefinition *getDatasetDefinition() override;

    bool datasetsAreAvailable();

private:
    Q_DISABLE_COPY(DatasetImportTab)

private slots:
    void selectedDatasetChanged(QString current);
};

#endif // DATASETIMPORTTAB_H
