#pragma once

#include <QDockWidget>

#include "DockTitleBar.h"

/// @brief Base class used by filters, data and plot docks.
class Dock : public QDockWidget
{
    Q_OBJECT
public:
    Dock(const QString& titleText, QWidget* parent);

protected:
    DockTitleBar titleBar_;

private slots:
    void toggleFloating();
};
