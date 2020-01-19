#include "DatasetInner.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include "Common/Constants.h"
#include "Common/ProgressBar.h"
#include "Shared/Logger.h"

#include "DatasetDefinitionInner.h"

const char* DatasetInner::datasetsDir_ = "Data";

DatasetInner::DatasetInner(DatasetDefinitionInner* datasetDefinition)
    : Dataset(datasetDefinition)
{

}

void DatasetInner::init()
{
    //If definition is not valid than skip loading data.
    if (datasetDefinition_->isValid())
    {
        data_.resize(datasetDefinition_->rowCount());

        const int activeColumnsCount = datasetDefinition_->getActiveColumnCount();
        for (int i = 0 ; i < datasetDefinition_->rowCount() ; ++i)
        {
            data_[i].resize(activeColumnsCount);
        }

        auto definition = dynamic_cast<DatasetDefinitionInner*>(datasetDefinition_);

        if (nullptr != definition && definition->getData(&data_))
        {
            valid_ = true;
            sharedStrings_ = definition->getSharedStringTable();
        }
    }
}

QStringList DatasetInner::getListOfAvailableDatasets()
{
    QDir datasetsDir(getDatasetsDir());

    if (!datasetDirExistAndUserHavePermisions())
    {
        return QStringList();
    }

    datasetsDir.setFilter(QDir::Files |
                          QDir::Readable |
                          QDir::NoSymLinks |
                          QDir::NoDotAndDotDot);
    datasetsDir.setNameFilters(QStringList("*" + Constants::getDatasetExtension()));
    datasetsDir.setSorting(QDir::Name);

    return datasetsDir.entryList().replaceInStrings(Constants::getDatasetExtension(), QLatin1String(""));
}

QString DatasetInner::getDatasetsDir()
{
    return QString(QApplication::applicationDirPath() + "/" + datasetsDir_ + "/");
}

bool DatasetInner::datasetDirExistAndUserHavePermisions()
{
    QDir directory(getDatasetsDir());

    if (!directory.exists())
    {
        bool created = directory.mkpath(directory.path());

        if (!created)
        {
            return false;
        }
    }

    return QFile::permissions(directory.path()).testFlag(QFile::ReadUser) &&
           QFile::permissions(directory.path()).testFlag(QFile::WriteUser);
}

bool DatasetInner::removeDataset(const QString& name)
{
    QString datasetFile =
        getDatasetsDir() + name + Constants::getDatasetExtension();
    return QFile::remove(datasetFile);
}
