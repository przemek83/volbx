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

    ~Dock() override = default;

public Q_SLOTS:
    void setNewToolTip(const QString& text);

protected:
    DockTitleBar titleBarWidget_;

private Q_SLOTS:
    void toggleFloating();
};
