#pragma once

#include <QDockWidget>

#include "DockTitleBar.h"

/**
 * @brief Base class used by filters, data and plot docks.
 */
class Dock : public QDockWidget
{
    Q_OBJECT
public:
    explicit Dock(const QString& titleText, QWidget* parent = nullptr,
                  Qt::WindowFlags flags = Qt::Widget);

protected:
    DockTitleBar titleBar_;

private Q_SLOTS:
    void toggleFloating();
};
