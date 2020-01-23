#include "HistogramPlotGui.h"

#include "ui_HistogramPlotGui.h"

HistogramPlotGui::HistogramPlotGui(QWidget* parent) :
    PlotDockWidget(tr("Histogram"), parent),
    ui(new Ui::HistogramPlotGui)
{
    ui->setupUi(this);

    setWindowTitle(tr("Histogram"));

    ui->verticalLayout->addWidget(&histogramPlot_);

    connect(ui->spinBox, qOverload<int>(&QSpinBox::valueChanged),
            &histogramPlot_, &HistogramPlot::recompute);
}

HistogramPlotGui::~HistogramPlotGui()
{
    delete ui;
}

void HistogramPlotGui::dataChanged(const PlotData& plotData, const Quantiles& quantiles)
{
    histogramPlot_.setNewData(plotData, quantiles, ui->spinBox->value());
}
