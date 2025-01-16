#pragma once

#include <QStringList>

namespace dataset_utilities
{
QString getDatasetsDir();

QStringList getListOfAvailableDatasets();

bool isDatasetDirUsable();

bool removeDataset(const QString& datasetName);

QString getDatasetDefinitionFilename();

QString getDatasetDataFilename();

QString getDatasetStringsFilename();

QString getDatasetExtension();

QString getDatasetNameRegExp();
};  // namespace dataset_utilities
