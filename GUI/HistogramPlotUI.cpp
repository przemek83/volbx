#include "HistogramPlotUI.h"

#include "ui_HistogramPlotUI.h"

HistogramPlotUI::HistogramPlotUI(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::HistogramPlotUI)
{
    ui->setupUi(this);

    setWindowTitle(tr("Histogram"));

    ui->verticalLayout->addWidget(&histogramPlot_);

    connect(ui->spinBox, qOverload<int>(&QSpinBox::valueChanged),
            &histogramPlot_, &HistogramPlot::recompute);
}

HistogramPlotUI::~HistogramPlotUI()
{
    delete ui;
}

void HistogramPlotUI::dataChanged(const PlotData& plotData, const Quantiles& quantiles)
{
    histogramPlot_.setNewData(plotData, quantiles, ui->spinBox->value());
}

