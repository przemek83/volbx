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
    explicit PlotDockWidget(QString title, QWidget* parent = nullptr,
                            Qt::WindowFlags flags = Qt::Widget);

    ~PlotDockWidget() override = default;

    QList<PlotBase*> exportContent();

private:
    Q_DISABLE_COPY(PlotDockWidget)

private slots:
    void quickExportData();

    void resetPlot();
};

#endif // PLOTDOCKWIDGET_H
