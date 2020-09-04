#include "PlotDock.h"

#include <PlotBase.h>
#include <QDebug>
#include <QPainter>
#include <QPushButton>

#include "Common/Constants.h"
#include "Export/ExportImage.h"

#include "DockTitleBar.h"

PlotDock::PlotDock(const QString& title, QWidget* parent, Qt::WindowFlags flags)
    : Dock(title, parent, flags)
{
    titleBarWidget_.setButtonVisible(DockTitleBar::Button::EXPORT, true);
    titleBarWidget_.setButtonVisible(DockTitleBar::Button::RESET, true);

    connect(&titleBarWidget_, &DockTitleBar::exportClicked, this,
            &PlotDock::quickExportData);
    connect(&titleBarWidget_, &DockTitleBar::resetClicked, this,
            &PlotDock::resetPlot);
}

void PlotDock::quickExportData()
{
    ExportImage::quickExportAsImage(exportContent());
}

void PlotDock::resetPlot()
{
    QList<PlotBase*> plots = findChildren<PlotBase*>();

    for (PlotBase* plot : plots)
        plot->resetPlot();
}

QList<PlotBase*> PlotDock::exportContent() { return findChildren<PlotBase*>(); }
