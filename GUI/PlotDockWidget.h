#pragma once

#include "Dock.h"

class PlotBase;

/**
 * @brief Dock widget for plots.
 */
class PlotDockWidget : public Dock
{
    Q_OBJECT
public:
    explicit PlotDockWidget(const QString& title, QWidget* parent = nullptr,
                            Qt::WindowFlags flags = Qt::Widget);

    ~PlotDockWidget() override = default;

    QList<PlotBase*> exportContent();

private Q_SLOTS:
    void quickExportData();

    void resetPlot();
};
