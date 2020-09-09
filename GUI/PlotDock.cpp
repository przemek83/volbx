#include "PlotDock.h"

#include <PlotBase.h>

#include <Export/ExportImage.h>

PlotDock::PlotDock(const QString& title, QWidget* parent, Qt::WindowFlags flags)
    : Dock(title, parent, flags)
{
    titleBar_.setButtonVisible(DockTitleBar::Button::EXPORT, true);
    titleBar_.setButtonVisible(DockTitleBar::Button::RESET, true);

    connect(&titleBar_, &DockTitleBar::exportClicked, this,
            &PlotDock::quickExportData);
    connect(&titleBar_, &DockTitleBar::resetClicked, this,
            &PlotDock::resetPlot);
}

void PlotDock::quickExportData() const
{
    ExportImage::quickExportAsImage(getPlots());
}

void PlotDock::resetPlot()
{
    QList<PlotBase*> plots{findChildren<PlotBase*>()};
    for (PlotBase* plot : plots)
        plot->resetPlot();
}

QList<PlotBase*> PlotDock::getPlots() const
{
    return findChildren<PlotBase*>();
}
