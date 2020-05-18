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
    explicit ViewDockWidget(const QString& title, QWidget* parent = nullptr,
                            Qt::WindowFlags flags = Qt::Widget);

    ~ViewDockWidget() override = default;

    void activateSelectButtons();

    void deactivateSelectButtons();

private Q_SLOTS:
    void quickExportData();

    void selectAll();

    void unselectAll();
};

#endif  // VIEWDOCKWIDGET_H
