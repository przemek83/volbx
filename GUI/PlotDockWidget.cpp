#include "PlotDockWidget.h"

#include <PlotBase.h>
#include <QDebug>
#include <QPainter>
#include <QPushButton>

#include "Common/Constants.h"
#include "Export/ExportImage.h"

#include "DockTitleBar.h"

PlotDockWidget::PlotDockWidget(const QString& title, QWidget* parent,
                               Qt::WindowFlags flags)
    : DockWidget(title, parent, flags)
{
    titleBarWidget_.setButtonVisible(DockTitleBar::Button::EXPORT, true);
    titleBarWidget_.setButtonVisible(DockTitleBar::Button::RESET, true);

    connect(&titleBarWidget_, &DockTitleBar::exportClicked, this,
            &PlotDockWidget::quickExportData);
    connect(&titleBarWidget_, &DockTitleBar::resetClicked, this,
            &PlotDockWidget::resetPlot);
}

void PlotDockWidget::quickExportData()
{
    ExportImage::quickExportAsImage(exportContent());
}

void PlotDockWidget::resetPlot()
{
    QList<PlotBase*> plots = findChildren<PlotBase*>();

    for (PlotBase* plot : plots)
        plot->resetPlot();
}

QList<PlotBase*> PlotDockWidget::exportContent()
{
    return findChildren<PlotBase*>();
}
