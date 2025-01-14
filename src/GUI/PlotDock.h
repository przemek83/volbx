#pragma once

#include "Dock.h"

class PlotBase;

class PlotDock : public Dock
{
    Q_OBJECT
public:
    PlotDock(const QString& title, QWidget* parent,
             Qt::WindowFlags flags = Qt::Widget);

    QList<PlotBase*> getPlots() const;

private Q_SLOTS:
    void quickExportData() const;

    void resetPlot();
};
