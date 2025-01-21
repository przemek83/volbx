#include "PlotDock.h"

#include <qwtble/PlotBase.h>

#include <Export/ExportImage.h>

PlotDock::PlotDock(const QString& title, QWidget* parent) : Dock(title, parent)
{
    const DockTitleBar& titleBar{getTitleBar()};
    titleBar.setButtonVisible(DockTitleBar::Button::EXPORT, true);
    titleBar.setButtonVisible(DockTitleBar::Button::RESET, true);

    connect(&titleBar, &DockTitleBar::exportClicked, this,
            &PlotDock::quickExportData);
    connect(&titleBar, &DockTitleBar::resetClicked, this, &PlotDock::resetPlot);
}

void PlotDock::quickExportData() const
{
    export_image::quickExportAsImage(getPlots());
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
