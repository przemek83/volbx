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

std::tuple<double, double> Dataset::getNumericRange(int column) const
{
    Q_ASSERT(DATA_FORMAT_FLOAT == getColumnFormat(column));

    double min {0.};
    double max {0.};

    bool first {true};
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

    return {min, max};
}

std::tuple<QDate, QDate, bool> Dataset::getDateRange(int column) const
{
    Q_ASSERT(DATA_FORMAT_DATE == getColumnFormat(column));

    QDate minDate;
    QDate maxDate;
    bool emptyDates {false};

    bool first {true};
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
            minDate = date;
            maxDate = date;
            first = false;
            continue;
        }

        if (date < minDate)
        {
            minDate = date;
        }

        if (date > maxDate)
        {
            maxDate = date;
        }
    }

    return {minDate, maxDate, emptyDates};
}

QStringList Dataset::getStringList(int column) const
{
    Q_ASSERT(DATA_FORMAT_STRING == getColumnFormat(column));

    QStringList listToFill;

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

    return listToFill;
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
