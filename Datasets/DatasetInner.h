#ifndef DATASETINNER_H
#define DATASETINNER_H

#include "Dataset.h"

class DatasetDefinitionInner;

/**
 * @brief Dataset class for inner format.
 */
class DatasetInner : public Dataset
{
public:
    explicit DatasetInner(DatasetDefinitionInner* datasetDefinition);

    ~DatasetInner() override = default;

    DatasetInner& operator=(const DatasetInner& other) = delete;
    DatasetInner(const DatasetInner& other) = delete;

    DatasetInner& operator=(DatasetInner&& other) = delete;
    DatasetInner(DatasetInner&& other) = delete;

    ///Return names without extension of available datasets in datasets dir.
    static QStringList getListOfAvailableDatasets();

    ///Returns datasets directory/folder.
    static QString getDatasetsDir();

    void init() override;

    ///Checks if it is possible to use default datasets directory to store data.
    static bool datasetDirExistAndUserHavePermisions();

    ///Removes given dataset from disk.
    static bool removeDataset(QString name);

private:
    ///Name of folder with datasets.
    static const char* datasetsDir_;
};

#endif // DATASETINNER_H
