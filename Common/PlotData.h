#ifndef PLOTDATA_H
#define PLOTDATA_H

#include <QObject>

/**
 * @brief Data used by plots.
 * Due to dynamic memory reference, counter implemented to properly use and
 * clear memory without thinking about it.
 */
class PlotData
{
public:
    PlotData(double* dataX, double* dataY, int dataSize);

    virtual ~PlotData();

    PlotData(const PlotData& right);
    PlotData& operator=(const PlotData& right);

    PlotData& operator=(PlotData&& other) = delete;
    PlotData(PlotData&& other) = delete;

    const double* getDataX() const;

    const double* getDataY() const;

    int getDataSize() const;

private:
    double* dataX_;

    double* dataY_;

    int dataSize_;

    class ReferenceCounter
    {
    public:
        ReferenceCounter() = default;

        ~ReferenceCounter() = default;

        void increase();

        int decrease();

        int current();

    private:
        Q_DISABLE_COPY(ReferenceCounter)

        int count_ {0};
    };

    ReferenceCounter* counter_;
};

#endif // PLOTDATA_H
