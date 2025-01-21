#include "Dock.h"

Dock::Dock(const QString& titleText, QWidget* parent)
    : QDockWidget(titleText, parent, Qt::Widget)
{
    titleBar_.setTitle(titleText);

    connect(&titleBar_, &DockTitleBar::closeClicked, this, &Dock::close);
    connect(&titleBar_, &DockTitleBar::floatingClicked, this,
            &Dock::toggleFloating);

    setTitleBarWidget(&titleBar_);
}

const DockTitleBar& Dock::getTitleBar() const { return titleBar_; }

void Dock::toggleFloating() { setFloating(!isFloating()); }
