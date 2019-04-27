#include <QScrollArea>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QSplitter>
#include <QScrollBar>

#include "Common/Constants.h"
#include "Common/SpecialColumns.h"
#include "ModelsAndViews/TableModel.h"
#include "Charts/GroupPlot.h"

#include "GroupPlotGui.h"
#include "ui_GroupPlotGui.h"
#include "DockTitleBar.h"

const char* GroupPlotGui::numRegExp_ =
    "^([\\d]+(,[\\d]+)?){1}(;([\\d]+(,[\\d]+)?))*$";

const char* GroupPlotGui::dateRegExp_ =
    "^(([1-9]|[12][0-9]|3[01])[- /.]([1-9]|1[012])[- /.](19|20)[0-9][0-9]?){1}(;((0?[1-9]|[12][0-9]|3[01])[- /.](0?[1-9]|1[012])[- /.](19|20)[0-9][0-9]))*$";

const char* GroupPlotGui::datePlaceHolder_ =
    QT_TRANSLATE_NOOP("GroupPlotGui", "dd/mm/yyyy;dd/mm/yyyy;dd/mm/yyyy...");

const char* GroupPlotGui::numPlaceHolder_ =
    QT_TRANSLATE_NOOP("GroupPlotGui", "number1;number2;number3...");

GroupPlotGui::GroupPlotGui(const TableModel* model, QWidget* parent) :
    PlotDockWidget(tr("Grouping"), parent),
    ui(new Ui::GroupPlotGui)
{
    ui->setupUi(this);

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

    numValidator_.setRegExp(QRegExp(QString(numRegExp_)));
    dateValidator_.setRegExp(QRegExp(QString(dateRegExp_)));

    ui->intervals->setPlaceholderText(tr(numPlaceHolder_));

    ui->intervals->setValidator(&numValidator_);

    for (int i = 0; i < model->columnCount(); ++i)
    {
        columnsNumberToFormatMap_[i] = model->getColumnFormat(i);

        if (DATA_FORMAT_STRING != model->getColumnFormat(i))
        {
            continue;
        }

        QString columnName = model->headerData(i, Qt::Horizontal).toString();
        ui->comboBox->addItem(columnName, QVariant(i));
    }

    ui->intervalsWidget->setVisible(false);
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
    groupPlot_.setAxisScale(QwtPlot::yLeft, minY, maxY);
    groupPlot_.setAxisScale(QwtPlot::yRight, minY, maxY);
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
    return columnsNumberToFormatMap_[columnNumber];
}

void GroupPlotGui::on_comboBox_currentIndexChanged(int index)
{
    ui->intervals->clear();

    switch (getSelectedColumnFormat())
    {
        case DATA_FORMAT_STRING:
        {
            ui->fromLabel->setText(QLatin1String(""));
            ui->toLabel->setText(QLatin1String(""));
            ui->intervals->setPlaceholderText(QLatin1String(""));
            ui->intervals->setEnabled(false);
            ui->changeIntervals->setEnabled(false);
            ui->intervalsLabel->setEnabled(false);
            ui->intervalsWidget->setVisible(false);
            break;
        }

        case DATA_FORMAT_FLOAT:
        {
            ui->intervals->setValidator(&numValidator_);
            ui->intervals->setPlaceholderText(tr(numPlaceHolder_));
            ui->intervals->setEnabled(true);
            ui->changeIntervals->setEnabled(true);
            ui->intervalsLabel->setEnabled(true);
            ui->intervalsWidget->setVisible(true);
            break;
        }

        case DATA_FORMAT_DATE:
        {
            ui->intervals->setValidator(&dateValidator_);
            ui->intervals->setPlaceholderText(tr(datePlaceHolder_));
            ui->intervals->setEnabled(true);
            ui->changeIntervals->setEnabled(true);
            ui->intervalsLabel->setEnabled(true);
            ui->intervalsWidget->setVisible(true);
            break;
        }

        default:
            break;
    }

    Q_EMIT newGroupingColumn(ui->comboBox->itemData(index).toInt());
}

GroupPlotGui::ScrollArea::ScrollArea(QWidget* parent)
    : QScrollArea(parent)
{

}

void GroupPlotGui::ScrollArea::forceResize()
{
    resizeEvent(new QResizeEvent(size(), size()));
}
