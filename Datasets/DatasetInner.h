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

    /// Return names without extension of available datasets in datasets dir.
    static QStringList getListOfAvailableDatasets();

    /// Returns datasets directory/folder.
    static QString getDatasetsDir();

    /// Checks if it is possible to use default datasets directory to store
    /// data.
    static bool datasetDirExistAndUserHavePermisions();

    /// Removes given dataset from disk.
    static bool removeDataset(const QString& datasetName);

protected:
    std::tuple<bool, QVector<QVector<QVariant>>> getSample() override;

    std::tuple<bool, QVector<QVector<QVariant>>> getAllData() override;

    bool analyze() override;

private:
    /// Load definition, strings and sample data.
    bool load();

    bool fromXml(QByteArray& definitionContent);

    bool loadXmlFile(QByteArray& definitionContent, QuaZip& zip);

    bool loadStrings(QuaZip& zip);

    std::tuple<bool, QVector<QVector<QVariant>>> fillData(QuaZip& zip,
                                                          bool fillSamplesOnly);

    void updateProgress(unsigned int currentRow, unsigned int rowCount,
                        unsigned int& lastEmittedPercent);

    void addElementToContainer(const ColumnType columnFormat,
                               const QString& element,
                               QVector<QVector<QVariant>>& dataContainer,
                               const int lineCounter,
                               const int columnToFill) const;

    QVariant getDefaultVariantForFormat(const ColumnType format) const;

    QVector<QVector<QVariant>> prepareContainerForData(
        bool fillSamplesOnly) const;

    QuaZip zip_;

    static const QString datasetsDir_;
};

#endif  // DATASETINNER_H
