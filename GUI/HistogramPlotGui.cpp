#include "HistogramPlotGui.h"
#include "ui_HistogramPlotGui.h"

HistogramPlotGui::HistogramPlotGui(QWidget* parent) :
    PlotDockWidget(tr("Histogram"), parent),
    ui(new Ui::HistogramPlotGui)
{
    ui->setupUi(this);

    setWindowTitle(tr("Histogram"));

    ui->verticalLayout->addWidget(&histogramPlot_);

    connect(ui->spinBox,
            SIGNAL(valueChanged(int)),
            &histogramPlot_,
            SLOT(recompute(int)));
}

HistogramPlotGui::~HistogramPlotGui()
{
    delete ui;
}

void HistogramPlotGui::dataChanged(const PlotData& plotData, const Quantiles& quantiles)
{
    histogramPlot_.setNewData(plotData, quantiles, ui->spinBox->value());
}
