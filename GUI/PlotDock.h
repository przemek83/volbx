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

    QList<PlotBase*> getPlots() const;

private Q_SLOTS:
    void quickExportData() const;

    void resetPlot();
};
