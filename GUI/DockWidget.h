#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QDockWidget>

#include "DockTitleBar.h"

/**
 * @brief Base class used by filters, data and plot docks.
 */
class DockWidget : public QDockWidget
{
    Q_OBJECT
public:
    DockWidget(QString titleText, QWidget* parent = nullptr,
               Qt::WindowFlags flags = Qt::Widget);

    ~DockWidget() override = default;

    DockWidget& operator=(const DockWidget& other) = delete;
    DockWidget(const DockWidget& other) = delete;

    DockWidget& operator=(DockWidget&& other) = delete;
    DockWidget(DockWidget&& other) = delete;

public slots:
    void setNewToolTip(QString text);

protected:
    DockTitleBar titleBarWidget_;

private slots:
    void manageFloating();
};

#endif // DOCKWIDGET_H
