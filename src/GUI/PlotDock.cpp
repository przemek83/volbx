#include "PlotDock.h"

#include <qwtble/PlotBase.h>

#include <Export/ExportImage.h>

PlotDock::PlotDock(const QString& title, QWidget* parent) : Dock(title, parent)
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

void PlotDock::resetPlot() const
{
    const QList<PlotBase*> plots{findChildren<PlotBase*>()};
    for (PlotBase* plot : plots)
        plot->resetPlot();
}

QList<PlotBase*> PlotDock::getPlots() const
{
    return findChildren<PlotBase*>();
}
