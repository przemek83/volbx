#ifndef DATASETINNER_H
#define DATASETINNER_H

#include "Dataset.h"

#include <Qt5Quazip/quazip.h>

/**
 * @brief Dataset class for inner format.
 */
class DatasetInner : public Dataset
{
public:
    explicit DatasetInner(const QString& name, QObject* parent = nullptr);

    ~DatasetInner() override = default;

    DatasetInner& operator=(const DatasetInner& other) = delete;
    DatasetInner(const DatasetInner& other) = delete;

    DatasetInner& operator=(DatasetInner&& other) = delete;
    DatasetInner(DatasetInner&& other) = delete;

    /// Return names without extension of available datasets in datasets dir.
    static QStringList getListOfAvailableDatasets();

    /// Returns datasets directory/folder.
    static QString getDatasetsDir();

    bool analyze() override;

    bool loadData() override;

    /// Checks if it is possible to use default datasets directory to store
    /// data.
    static bool datasetDirExistAndUserHavePermisions();

    /// Removes given dataset from disk.
    static bool removeDataset(const QString& name);

protected:
    std::unique_ptr<QVariant[]> getSharedStringTable() override;

private:
    void updateSampleDataStrings();

    /// Load definition, strings and sample data.
    bool load();

    // Parse given xml and fill inner definition containers.
    bool fromXml(QByteArray& definitionContent);

    /// Load definition file from zip.
    bool loadXmlFile(QByteArray& definitionContent, QuaZip& zip);

    /// Load strings from zip file.
    bool loadStrings(QuaZip& zip);

    bool fillData(QuaZip& zip, QVector<QVector<QVariant> >& dataContainer,
                  bool fillSamplesOnly);

    void updateProgress(unsigned int currentRow, unsigned int rowCount,
                        unsigned int& lastEmittedPercent);

    /// Add current element into given container.
    void addElementToContainer(const ColumnType columnFormat,
                               const QString& element,
                               QVector<QVector<QVariant> >& dataContainer,
                               const int lineCounter,
                               const int columnToFill) const;

    /// Array with strings.
    std::unique_ptr<QVariant[]> stringsTable_{nullptr};

    /// Zip file.
    QuaZip zip_;

    /// Name of folder with datasets.
    static const char* datasetsDir_;
};

#endif  // DATASETINNER_H
