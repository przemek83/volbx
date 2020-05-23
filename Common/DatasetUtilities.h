#ifndef DATASETUTILITIES_H
#define DATASETUTILITIES_H

#include <QStringList>

namespace DatasetUtilities
{
QString getDatasetsDir();

/// Return names without extension of available datasets in datasets dir.
QStringList getListOfAvailableDatasets();

/// Checks if it is possible to use default datasets directory to store
/// data.
bool isDatasetDirExistAndUserHavePermisions();

/// Removes given dataset from disk.
bool removeDataset(const QString& datasetName);

QString getDatasetDefinitionFilename();

QString getDatasetDataFilename();

QString getDatasetStringsFilename();

QString getDatasetExtension();

QString getDatasetNameRegExp();
};  // namespace DatasetUtilities

#endif  // DATASETUTILITIES_H
