#ifndef HISTOGRAMPLOTGUI_H
#define HISTOGRAMPLOTGUI_H

#include "PlotDockWidget.h"
#include "Charts/HistogramPlot.h"

namespace Ui {
class HistogramPlotGui;
}

/**
 * @brief Gui dock containing histogram.
 */
class HistogramPlotGui : public PlotDockWidget
{
    Q_OBJECT
public:
    explicit HistogramPlotGui(QWidget* parent = 0);

    virtual ~HistogramPlotGui();

private:
    Q_DISABLE_COPY(HistogramPlotGui)

    Ui::HistogramPlotGui *ui;

    HistogramPlot histogramPlot_;

private slots:
    void dataChanged(PlotData plotData, Quantiles quantiles);
};

#endif // HISTOGRAMPLOTGUI_H
