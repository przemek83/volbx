#include "GroupPlotGui.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QSplitter>
#include <QtAlgorithms>

#include "Charts/GroupPlot.h"
#include "Common/Constants.h"
#include "Common/SpecialColumns.h"
#include "ModelsAndViews/TableModel.h"

#include "DockTitleBar.h"
#include "ui_GroupPlotGui.h"

GroupPlotGui::GroupPlotGui(const TableModel* model, QWidget* parent) :
    PlotDockWidget(tr("Grouping"), parent),
    ui(new Ui::GroupPlotGui)
{
    ui->setupUi(this);

    connect(ui->comboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &GroupPlotGui::comboBoxCurrentIndexChanged);

    dynamic_cast<DockTitleBar*>(titleBarWidget())->setTitle(windowTitle());

    //Add splitter with group and quantiles plots.
    auto splitter = new QSplitter(Qt::Horizontal, this);
    scrollArea_ = new ScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setWidget(&groupPlot_);
    splitter->addWidget(scrollArea_);
    splitter->addWidget(&quantilesPlot_);
    ui->verticalLayout->insertWidget(2, splitter, 0);

    ui->comboBox->clear();

    for (int i = 0; i < model->columnCount(); ++i)
    {
        columnsNumberToFormatMap_[i] = model->getColumnFormat(i);

        // Accept only string type columns.
        if (DATA_FORMAT_STRING != model->getColumnFormat(i))
        {
            continue;
        }

        QString columnName = model->headerData(i, Qt::Horizontal).toString();
        ui->comboBox->addItem(columnName, QVariant(i));
    }
}

GroupPlotGui::~GroupPlotGui()
{
    delete ui;
}

void GroupPlotGui::setNewData(float minY,
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

DataFormat GroupPlotGui::getSelectedColumnFormat()
{
    int columnNumber = ui->comboBox->itemData(ui->comboBox->currentIndex()).toInt();
    return columnsNumberToFormatMap_.value(columnNumber);
}

void GroupPlotGui::comboBoxCurrentIndexChanged(int index)
{
    Q_EMIT newGroupingColumn(ui->comboBox->itemData(index).toInt());
}
