#ifndef HISTOGRAMPLOTGUI_H
#define HISTOGRAMPLOTGUI_H

#include <HistogramPlot.h>

#include "PlotDockWidget.h"

namespace Ui
{
class HistogramPlotGui;
} // namespace Ui

/**
 * @brief Gui dock containing histogram.
 */
class HistogramPlotGui : public PlotDockWidget
{
    Q_OBJECT
public:
    explicit HistogramPlotGui(QWidget* parent = nullptr);

    ~HistogramPlotGui() override;

    HistogramPlotGui& operator=(const HistogramPlotGui& other) = delete;
    HistogramPlotGui(const HistogramPlotGui& other) = delete;

    HistogramPlotGui& operator=(HistogramPlotGui&& other) = delete;
    HistogramPlotGui(HistogramPlotGui&& other) = delete;

public Q_SLOTS:
    void dataChanged(const PlotData& plotData, const Quantiles& quantiles);

private:
    Ui::HistogramPlotGui* ui;

    HistogramPlot histogramPlot_;
};

#endif // HISTOGRAMPLOTGUI_H
