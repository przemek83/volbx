#include "GroupPlotUI.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSplitter>
#include <QtAlgorithms>

#include "DockTitleBar.h"
#include "ScrollArea.h"
#include "ui_GroupPlotUI.h"

GroupPlotUI::GroupPlotUI(QVector<std::pair<QString, int> > stringColumns, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GroupPlotUI)
{
    ui->setupUi(this);

    connect(ui->comboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &GroupPlotUI::comboBoxCurrentIndexChanged);

    //Add splitter with group and quantiles plots.
    auto splitter = new QSplitter(Qt::Horizontal, this);
    scrollArea_ = new ScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setWidget(&groupPlot_);
    splitter->addWidget(scrollArea_);
    splitter->addWidget(&quantilesPlot_);
    ui->verticalLayout->insertWidget(2, splitter, 0);

    ui->comboBox->clear();

    for (const auto& [columnName, index] : stringColumns)
        ui->comboBox->addItem(columnName, QVariant(index));
}

GroupPlotUI::~GroupPlotUI()
{
    delete ui;
}

void GroupPlotUI::setNewData(float minY,
                             float maxY,
                             QVector<QString> intervalsNames,
                             const QVector<Quantiles>& quantilesForIntervals,
                             const Quantiles& quantiles)
{
    groupPlot_.setAxisScale(QwtPlot::yLeft, static_cast<double>(minY), static_cast<double>(maxY));
    groupPlot_.setAxisScale(QwtPlot::yRight, static_cast<double>(minY), static_cast<double>(maxY));
    groupPlot_.setAxisScale(QwtPlot::xBottom, 0, intervalsNames.size() + 1, 1);

    groupPlot_.setNewData(quantilesForIntervals,
                          intervalsNames);

    quantilesPlot_.setNewData(quantiles);

    scrollArea_->forceResize();

    int scrollBarSize = 0;

    if (scrollArea_->horizontalScrollBar()->minimum() !=
        scrollArea_->horizontalScrollBar()->maximum())
    {
        scrollBarSize = scrollArea_->horizontalScrollBar()->height();
    }

    //Problem when scrollbar was visible and after resize is not.
    //Quantiles plot not resized.
    double minExtentForQuantiles =
        groupPlot_.axisScaleDraw(QwtPlot::xBottom)->extent(groupPlot_.axisFont(QwtPlot::xBottom));
    groupPlot_.axisScaleDraw(QwtPlot::xBottom)->setMinimumExtent(minExtentForQuantiles);

    quantilesPlot_.axisScaleDraw(QwtPlot::xBottom)->setMinimumExtent(minExtentForQuantiles + scrollBarSize);
    quantilesPlot_.forceResize();
}

void GroupPlotUI::comboBoxCurrentIndexChanged(int index)
{
    Q_EMIT newGroupingColumn(ui->comboBox->itemData(index).toInt());
}
