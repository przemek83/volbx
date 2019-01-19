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
    DockWidget(QString titleText,
               QWidget* parent = 0,
               Qt::WindowFlags flags = 0);

    virtual ~DockWidget();

public slots:
    void setNewToolTip(QString text);

protected:
    DockTitleBar titleBarWidget_;

private:
    Q_DISABLE_COPY(DockWidget)

private slots:
    void manageFloating();
};

#endif // DOCKWIDGET_H
