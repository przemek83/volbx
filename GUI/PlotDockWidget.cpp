#include <QPushButton>
#include <QDebug>
#include <QPainter>
#include <QScrollArea>

#include "Common/ExportImage.h"
#include "Common/Constants.h"
#include "Charts/PlotBase.h"

#include "PlotDockWidget.h"
#include "DockTitleBar.h"

PlotDockWidget::PlotDockWidget(QString title,
                               QWidget* parent,
                               Qt::WindowFlags flags)
    : DockWidget(title, parent, flags)
{
    titleBarWidget_.getExportButton()->setVisible(true);
    titleBarWidget_.getResetButton()->setVisible(true);

    connect(titleBarWidget_.getExportButton(),
            SIGNAL(clicked()),
            this,
            SLOT(quickExportData()));
    connect(titleBarWidget_.getResetButton(),
            SIGNAL(clicked()),
            this,
            SLOT(resetPlot()));
}

void PlotDockWidget::quickExportData()
{
    ExportImage::quickExportAsImage(exportContent());
}

void PlotDockWidget::resetPlot()
{
    QList<PlotBase*> plots = findChildren<PlotBase*>();

    for (PlotBase* plot : plots)
    {
        plot->resetPlot();
    }
}

QList<PlotBase*> PlotDockWidget::exportContent()
{
    return findChildren<PlotBase*>();
}
