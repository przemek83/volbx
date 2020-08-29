#pragma once

#include <memory>

#include <QString>

class QAbstractItemView;
class Dataset;
class DatasetSpreadsheet;

/**
 * @brief Common functions for unit tests.
 */
namespace Common
{
void saveFile(const QString& fileName, const QString& data);

QString getDefinitionDumpSuffix();

QString getDefinitionRefSuffix();

QString getDataTsvDumpSuffix();

QString getExportedTsv(const QAbstractItemView& view);

bool xmlsAreEqual(const QByteArray& left, const QByteArray& right);

void compareExportDataWithDump(std::unique_ptr<Dataset> dataset);

std::unique_ptr<DatasetSpreadsheet> createDataset(const QString& filePath);

QString getSpreadsheetsDir();

void activateAllDatasetColumns(Dataset& dataset);
};  // namespace Common
