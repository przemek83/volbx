#ifndef VIEWDOCKWIDGET_H
#define VIEWDOCKWIDGET_H

#include "DockWidget.h"

class QPushButton;

/**
 * @brief dock widget for data view.
 */
class ViewDockWidget : public DockWidget
{
    Q_OBJECT
public:
    ViewDockWidget(QString title,  QWidget* parent = 0, Qt::WindowFlags flags = 0);

    virtual ~ViewDockWidget();

    void activateSelectButtons();

    void deactivateSelectButtons();

private:
    Q_DISABLE_COPY(ViewDockWidget)

private slots:
    void quickExportData();

    void selectAll();

    void unselectAll();
};

#endif // VIEWDOCKWIDGET_H
