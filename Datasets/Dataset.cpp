#include <QVariant>
#include <vector>
#include <QDate>
#include <QDebug>
#include <QStringList>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTime>

#include "Common/Constants.h"
#include "Shared/Logger.h"
#include "Common/ProgressBar.h"

#include "Dataset.h"

Dataset::Dataset(DatasetDefinition* definition) :
    datasetDefinition_(definition),
    data_(QVector<QVector<QVariant> >()),
    valid_(false),
    sharedStrings_(nullptr)
{

}

Dataset::~Dataset()
{
    delete datasetDefinition_;

    if( nullptr != sharedStrings_)
    {
        delete[] sharedStrings_;
    }
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
    for(int i = 0; i < rowCount(); ++i)
    {
        double value = data_[i][column].toDouble();
        if(first)
        {
            min = value;
            max = value;
            first = false;
            continue;
        }

        if(value < min) {
            min = value;
}

        if(value > max) {
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
    for( int i = 0; i < rowCount(); ++i )
    {
        const QVariant& dateVariant = data_[i][column];

        if( true == dateVariant.isNull() )
        {
            emptyDates = true;
            continue;
        }

        QDate date = dateVariant.toDate();

        if( first )
        {
            min = date;
            max = date;
            first = false;
            continue;
        }

        if( date < min ) {
            min = date;
}

        if( date > max ) {
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
    for(int i = 0; i < rowCount(); ++i) {
        listToFill.append(data_[i][column].toString());
}

    listToFill.removeDuplicates();

    if( nullptr != sharedStrings_ )
    {
        int listSize = listToFill.count();
        for( int i = 0; i < listSize; ++i )
        {
            listToFill[i] = sharedStrings_[listToFill[i].toInt()].toString();
        }
    }
}

bool Dataset::getSpecialColumnIfExists(SpecialColumn columnTag, int& column) const
{
    return datasetDefinition_->getSpecialColumnIfExists(columnTag, column);
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

    int column = 0;
    bool exist = getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT, column);

    if ( true == exist )
    {
        tabName.append(" (" + getHeaderName(column) + ")");
    }

    return tabName;
}
