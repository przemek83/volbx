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
    titleBar_.setButtonVisible(DockTitleBar::Button::EXPORT, true);
    titleBar_.setButtonVisible(DockTitleBar::Button::RESET, true);

    connect(&titleBar_, &DockTitleBar::exportClicked, this,
            &PlotDock::quickExportData);
    connect(&titleBar_, &DockTitleBar::resetClicked, this,
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
