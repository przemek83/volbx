#ifndef HISTOGRAMPLOTUI_H
#define HISTOGRAMPLOTUI_H

#include <HistogramPlot.h>
#include <QWidget>

namespace Ui
{
class HistogramPlotUI;
}

class HistogramPlotUI : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramPlotUI(QWidget* parent = nullptr);
    ~HistogramPlotUI() override;

    HistogramPlotUI& operator=(const HistogramPlotUI& other) = delete;
    HistogramPlotUI(const HistogramPlotUI& other) = delete;

    HistogramPlotUI& operator=(HistogramPlotUI&& other) = delete;
    HistogramPlotUI(HistogramPlotUI&& other) = delete;

public Q_SLOTS:
    void dataChanged(const PlotData& plotData, const Quantiles& quantiles);

private:
    Ui::HistogramPlotUI* ui;

    HistogramPlot histogramPlot_;
};

#endif // HISTOGRAMPLOTUI_H
