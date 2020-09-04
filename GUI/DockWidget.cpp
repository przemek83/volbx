#include "DockWidget.h"

DockWidget::DockWidget(const QString& titleText, QWidget* parent,
                       Qt::WindowFlags flags)
    : QDockWidget(titleText, parent, flags)
{
    titleBarWidget_.setTitle(titleText);

    connect(&titleBarWidget_, &DockTitleBar::closeClicked, this,
            &DockWidget::close);
    connect(&titleBarWidget_, &DockTitleBar::floatingClicked, this,
            &DockWidget::toggleFloating);

    setTitleBarWidget(&titleBarWidget_);
}

void DockWidget::setNewToolTip(const QString& text) { setToolTip(text); }

void DockWidget::toggleFloating() { setFloating(!isFloating()); }
