#include "Dataset.h"

#include <vector>

#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTime>
#include <QVariant>

#include "Common/Constants.h"
#include "Common/ProgressBar.h"
#include "Shared/Logger.h"

Dataset::Dataset(DatasetDefinition* definition) :
    datasetDefinition_(definition)
{

}

Dataset::~Dataset()
{
    delete datasetDefinition_;
}

int Dataset::rowCount() const
{
    return datasetDefinition_->rowCount();
}

int Dataset::columnCount() const
{
    return datasetDefinition_->columnCount();
}

DataFormat Dataset::getColumnFormat(int column) const
{
    return datasetDefinition_->getColumnFormat(column);
}

void Dataset::getNumericRange(int column, double& min, double& max) const
{
    Q_ASSERT(DATA_FORMAT_FLOAT == getColumnFormat(column));

    bool first = true;
    for (int i = 0; i < rowCount(); ++i)
    {
        double value = data_[i][column].toDouble();
        if (first)
        {
            min = value;
            max = value;
            first = false;
            continue;
        }

        if (value < min)
        {
            min = value;
        }

        if (value > max)
        {
            max = value;
        }
    }
}

void Dataset::getDateRange(int column,
                           QDate& min,
                           QDate& max,
                           bool& emptyDates) const
{
    Q_ASSERT(DATA_FORMAT_DATE == getColumnFormat(column));

    bool first = true;
    for (int i = 0; i < rowCount(); ++i)
    {
        const QVariant& dateVariant = data_[i][column];

        if (dateVariant.isNull())
        {
            emptyDates = true;
            continue;
        }

        QDate date = dateVariant.toDate();

        if (first)
        {
            min = date;
            max = date;
            first = false;
            continue;
        }

        if (date < min)
        {
            min = date;
        }

        if (date > max)
        {
            max = date;
        }
    }
}

void Dataset::getStringList(int column, QStringList& listToFill) const
{
    Q_ASSERT(DATA_FORMAT_STRING == getColumnFormat(column));

    listToFill.clear();

    //Optimization used -> use string indexes first, compare, remove duplicates.
    //At end convert to proper strings.
    for (int i = 0; i < rowCount(); ++i)
    {
        listToFill.append(data_[i][column].toString());
    }

    listToFill.removeDuplicates();

    if (nullptr != sharedStrings_)
    {
        int listSize = listToFill.count();
        for (int i = 0; i < listSize; ++i)
        {
            listToFill[i] = sharedStrings_[listToFill[i].toULongLong()].toString();
        }
    }
}

std::tuple<bool, int> Dataset::getSpecialColumnIfExists(SpecialColumn columnTag) const
{
    return datasetDefinition_->getSpecialColumnIfExists(columnTag);
}

QString Dataset::getHeaderName(int column) const
{
    return datasetDefinition_->getColumnName(column);
}

bool Dataset::isValid() const
{
    return valid_;
}

const DatasetDefinition* Dataset::getDatasetDefinition() const
{
    return datasetDefinition_;
}

QString Dataset::getName() const
{
    return datasetDefinition_->getName();
}

QString Dataset::getNameForTabBar()
{
    QString tabName = getName();

    if (auto [ok, column] = getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT); ok)
    {
        tabName.append(" (" + getHeaderName(column) + ")");
    }

    return tabName;
}
