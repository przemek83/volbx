#ifndef PLOTDOCKWIDGET_H
#define PLOTDOCKWIDGET_H

#include "DockWidget.h"

class PlotBase;

/**
 * @brief Dock widget for plots.
 */
class PlotDockWidget :  public DockWidget
{
    Q_OBJECT
public:
    explicit PlotDockWidget(const QString& title, QWidget* parent = nullptr,
                            Qt::WindowFlags flags = Qt::Widget);

    ~PlotDockWidget() override = default;

    PlotDockWidget& operator=(const PlotDockWidget& other) = delete;
    PlotDockWidget(const PlotDockWidget& other) = delete;

    PlotDockWidget& operator=(PlotDockWidget&& other) = delete;
    PlotDockWidget(PlotDockWidget&& other) = delete;

    QList<PlotBase*> exportContent();

private Q_SLOTS:
    void quickExportData();

    void resetPlot();
};

#endif // PLOTDOCKWIDGET_H
