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
    explicit DatasetImportTab(QWidget *parent = 0);

    virtual ~DatasetImportTab() = default;

    virtual DatasetDefinition* getDatasetDefinition();

    bool datasetsAreAvailable();

private:
    Q_DISABLE_COPY(DatasetImportTab)

private slots:
    void selectedDatasetChanged(QString current);
};

#endif // DATASETIMPORTTAB_H
