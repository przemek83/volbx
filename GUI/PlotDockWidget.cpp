#include "PlotDockWidget.h"

#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>

#include "Charts/PlotBase.h"
#include "Common/Constants.h"
#include "Common/ExportImage.h"

#include "DockTitleBar.h"

PlotDockWidget::PlotDockWidget(const QString& title,
                               QWidget* parent,
                               Qt::WindowFlags flags)
    : DockWidget(title, parent, flags)
{
    titleBarWidget_.getExportButton()->setVisible(true);
    titleBarWidget_.getResetButton()->setVisible(true);

    connect(titleBarWidget_.getExportButton(), &QPushButton::clicked,
            this, &PlotDockWidget::quickExportData);
    connect(titleBarWidget_.getResetButton(), &QPushButton::clicked,
            this, &PlotDockWidget::resetPlot);
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
