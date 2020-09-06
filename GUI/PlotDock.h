#pragma once

#include "Dock.h"

class PlotBase;

/**
 * @brief Dock widget with plots.
 */
class PlotDock : public Dock
{
    Q_OBJECT
public:
    explicit PlotDock(const QString& title, QWidget* parent = nullptr,
                      Qt::WindowFlags flags = Qt::Widget);

    ~PlotDock() override = default;

    QList<PlotBase*> getPlots();

private Q_SLOTS:
    void quickExportData();

    void resetPlot();
};
