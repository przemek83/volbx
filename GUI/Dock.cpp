#include "Dock.h"

Dock::Dock(const QString& titleText, QWidget* parent,
                       Qt::WindowFlags flags)
    : QDockWidget(titleText, parent, flags)
{
    titleBarWidget_.setTitle(titleText);

    connect(&titleBarWidget_, &DockTitleBar::closeClicked, this,
            &Dock::close);
    connect(&titleBarWidget_, &DockTitleBar::floatingClicked, this,
            &Dock::toggleFloating);

    setTitleBarWidget(&titleBarWidget_);
}

void Dock::setNewToolTip(const QString& text) { setToolTip(text); }

void Dock::toggleFloating() { setFloating(!isFloating()); }
