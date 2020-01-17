#include "PlotMagnifier.h"

PlotMagnifier::PlotMagnifier(QWidget* canvas)
    : QwtPlotMagnifier(canvas)
{
    actualFactor_ = 1.0;
}

void PlotMagnifier::rescale(double factor)
{
    actualFactor_ *= factor;
    QwtPlotMagnifier::rescale(factor);
}

void PlotMagnifier::reset()
{
    rescale(1 / actualFactor_);
    actualFactor_ = 1.0;
}
