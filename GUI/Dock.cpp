#include "Dock.h"

Dock::Dock(const QString& titleText, QWidget* parent,
                       Qt::WindowFlags flags)
    : QDockWidget(titleText, parent, flags)
{
    titleBar_.setTitle(titleText);

    connect(&titleBar_, &DockTitleBar::closeClicked, this,
            &Dock::close);
    connect(&titleBar_, &DockTitleBar::floatingClicked, this,
            &Dock::toggleFloating);

    setTitleBarWidget(&titleBar_);
}

void Dock::toggleFloating() { setFloating(!isFloating()); }
