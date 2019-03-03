#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QStandardPaths>

#include "Common/Constants.h"
#include "Shared/Logger.h"
#include "Common/ProgressBar.h"

#include "DatasetDefinitionInner.h"
#include "DatasetInner.h"

const char* DatasetInner::datasetsDir_ = "Data";

DatasetInner::DatasetInner(DatasetDefinitionInner* datasetDefinition)
    : Dataset(static_cast<DatasetDefinition*>(datasetDefinition))
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

        if (nullptr != definition && true == definition->getData(&data_))
        {
            valid_ = true;
            sharedStrings_ = definition->getSharedStringTable();
        }
    }
}

QStringList DatasetInner::getListOfAvailableDatasets()
{
    QDir datasetsDir(getDatasetsDir());

    if (false == datasetDirExistAndUserHavePermisions())
    {
        return QStringList();
    }

    datasetsDir.setFilter(QDir::Files |
                          QDir::Readable |
                          QDir::NoSymLinks |
                          QDir::NoDotAndDotDot);
    datasetsDir.setNameFilters(QStringList("*" + QString(Constants::datasetExtension_)));
    datasetsDir.setSorting(QDir::Name);

    return datasetsDir.entryList().replaceInStrings(QString(Constants::datasetExtension_), "");
}

QString DatasetInner::getDatasetsDir()
{
    return QString(QApplication::applicationDirPath() + "/" + datasetsDir_ + "/");
}

bool DatasetInner::datasetDirExistAndUserHavePermisions()
{
    QDir directory(getDatasetsDir());

    if (false == directory.exists())
    {
        bool created = directory.mkpath(directory.path());

        if (false == created)
        {
            return false;
        }
    }

    if (true == QFile::permissions(directory.path()).testFlag(QFile::ReadUser) &&
        true == QFile::permissions(directory.path()).testFlag(QFile::WriteUser))
    {
        return true;
    }

    return false;
}

bool DatasetInner::removeDataset(QString name)
{
    QString datasetFile =
        getDatasetsDir() + name + QString(Constants::datasetExtension_);
    return QFile::remove(datasetFile);
}
