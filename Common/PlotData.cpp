#include <QDebug>

#include "PlotData.h"

PlotData::PlotData(double* dataX, double* dataY, int dataSize):
    dataX_(dataX),
    dataY_(dataY),
    dataSize_(dataSize),
    counter_(new ReferenceCounter())
{
    counter_->increase();
}

PlotData::~PlotData()
{
    if (0 >= counter_->decrease())
    {
        delete[] dataX_;
        delete[] dataY_;
        delete counter_;
    }
}

PlotData::PlotData(const PlotData& right):
    dataX_(right.dataX_),
    dataY_(right.dataY_),
    counter_(right.counter_),
    dataSize_(right.dataSize_)
{
    counter_->increase();
}

PlotData& PlotData::operator=(const PlotData& right)
{
    //Avoid self assignment
    if (this != &right)
    {
        //Decrement the old reference count
        //if reference become zero delete the old data.
        if (counter_->decrease() == 0)
        {
            delete[] dataX_;
            delete[] dataY_;
            delete counter_;
        }

        //Copy the data and reference pointer.
        dataX_ = right.dataX_;
        dataY_ = right.dataY_;
        dataSize_ = right.dataSize_;
        counter_ = right.counter_;

        //Increment the reference count.
        counter_->increase();
    }
    return *this;
}

const double* PlotData::getDataX() const
{
    return dataX_;
}

const double* PlotData::getDataY() const
{
    return dataY_;
}

int PlotData::getDataSize() const
{
    return dataSize_;
}

void PlotData::ReferenceCounter::increase()
{
    count_++;
}

int PlotData::ReferenceCounter::decrease()
{
    return --count_;
}

int PlotData::ReferenceCounter::current()
{
    return count_;
}
