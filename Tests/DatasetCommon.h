#pragma once

#include <memory>

class Dataset;
class QString;
class QAbstractItemView;
class QByteArray;

namespace DatasetCommon
{
bool xmlsAreEqual(const QByteArray& left, const QByteArray& right);

QString getExportedTsv(const QAbstractItemView& view);

void compareExportDataWithDump(std::unique_ptr<Dataset> dataset,
                               const QString& filePath);

std::unique_ptr<Dataset> createDataset(const QString& fileName,
                                       const QString& filePath);

void activateAllDatasetColumns(Dataset& dataset);

void checkDefinition(const QString& fileName, const QString& filePath);

void checkData(const QString& fileName, const QString& dir);

void generateExpectedDataForFile(const QString& fileName,
                                 const QString& sourceDir,
                                 const QString& destinationDir);
};  // namespace DatasetCommon
