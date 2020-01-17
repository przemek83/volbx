#ifndef PLOTMAGNIFIER_H
#define PLOTMAGNIFIER_H

#include <qwt_plot_magnifier.h>

class PlotMagnifier : public QwtPlotMagnifier
{
    Q_OBJECT
public:
    explicit PlotMagnifier(QWidget* canvas);

    virtual ~PlotMagnifier() override = default;

    PlotMagnifier& operator=(const PlotMagnifier& other) = delete;
    PlotMagnifier(const PlotMagnifier& other) = delete;

    PlotMagnifier& operator=(PlotMagnifier&& other) = delete;
    PlotMagnifier(PlotMagnifier&& other) = delete;

    void reset();

protected:
    void rescale(double factor) override;

private:
    double actualFactor_;
};


#endif // PLOTMAGNIFIER_H
