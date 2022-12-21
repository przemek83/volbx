#include "DatasetUtilities.h"

#include <QApplication>
#include <QDir>
#include <QFile>

namespace DatasetUtilities
{
QString getDatasetsDir()
{
    const QString datasetsDirName{QStringLiteral("Data")};
    return QString(QApplication::applicationDirPath() + "/" + datasetsDirName +
                   "/");
}

QStringList getListOfAvailableDatasets()
{
    QDir datasetsDir{getDatasetsDir()};
    if (!doesDatasetDirExistAndUserHavePermisions())
        return {};

    datasetsDir.setFilter(QDir::Files | QDir::Readable | QDir::NoSymLinks |
                          QDir::NoDotAndDotDot);
    datasetsDir.setNameFilters(QStringList("*" + getDatasetExtension()));
    datasetsDir.setSorting(QDir::Name);

    QStringList entries{datasetsDir.entryList()};
    return entries.replaceInStrings(getDatasetExtension(), QStringLiteral(""));
}

bool doesDatasetDirExistAndUserHavePermisions()
{
    const QDir directory{getDatasetsDir()};
    if (!directory.exists() && !directory.mkpath(directory.path()))
        return false;

    return QFile::permissions(directory.path()).testFlag(QFile::ReadUser) &&
           QFile::permissions(directory.path()).testFlag(QFile::WriteUser);
}

bool removeDataset(const QString& datasetName)
{
    const QString datasetFile{getDatasetsDir() + datasetName +
                              getDatasetExtension()};
    return QFile::remove(datasetFile);
}

QString getDatasetDefinitionFilename()
{
    return QStringLiteral("definition.xml");
}

QString getDatasetDataFilename() { return QStringLiteral("data.csv"); }

QString getDatasetStringsFilename() { return QStringLiteral("strings.txt"); }

QString getDatasetExtension() { return QStringLiteral(".vbx"); }

QString getDatasetNameRegExp() { return QStringLiteral("[\\w\\s-]+"); }

}  // namespace DatasetUtilities
