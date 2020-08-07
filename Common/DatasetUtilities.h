#ifndef DATASETUTILITIES_H
#define DATASETUTILITIES_H

#include <QStringList>

/**
 * Helper functions for dataset related operations.
 */
namespace DatasetUtilities
{
QString getDatasetsDir();

/// Return names without extension of available datasets in datasets dir.
QStringList getListOfAvailableDatasets();

/// Checks if default datasets directory can be used.
bool doesDatasetDirExistAndUserHavePermisions();

/// Removes given dataset from disk.
bool removeDataset(const QString& datasetName);

QString getDatasetDefinitionFilename();

QString getDatasetDataFilename();

QString getDatasetStringsFilename();

QString getDatasetExtension();

QString getDatasetNameRegExp();
};  // namespace DatasetUtilities

#endif  // DATASETUTILITIES_H
