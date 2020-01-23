#include "DockWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>

#include "Common/Constants.h"
#include "ModelsAndViews/DataView.h"

DockWidget::DockWidget(const QString& titleText,
                       QWidget* parent,
                       Qt::WindowFlags flags)
    : QDockWidget(titleText, parent, flags)
{
    titleBarWidget_.setTitle(titleText);

    connect(titleBarWidget_.getCloseButton(), &QPushButton::clicked, this, &DockWidget::close);
    connect(titleBarWidget_.getFloatingButton(), &QPushButton::clicked, this, &DockWidget::manageFloating);

    setTitleBarWidget(&titleBarWidget_);
}

void DockWidget::setNewToolTip(const QString& text)
{
    setToolTip(text);
}

void DockWidget::manageFloating()
{
    setFloating(!isFloating());
}
