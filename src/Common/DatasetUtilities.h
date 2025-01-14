#pragma once

#include <QStringList>

namespace DatasetUtilities
{
QString getDatasetsDir();

QStringList getListOfAvailableDatasets();

bool doesDatasetDirExistAndUserHavePermisions();

bool removeDataset(const QString& datasetName);

QString getDatasetDefinitionFilename();

QString getDatasetDataFilename();

QString getDatasetStringsFilename();

QString getDatasetExtension();

QString getDatasetNameRegExp();
};  // namespace DatasetUtilities
