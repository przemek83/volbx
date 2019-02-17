#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>

#include "Common/Constants.h"

#include "DockWidget.h"
#include "ModelsAndViews/DataView.h"

DockWidget::DockWidget(QString titleText,
                       QWidget* parent,
                       Qt::WindowFlags flags)
    : QDockWidget(titleText, parent, flags)
{
    titleBarWidget_.setTitle(titleText);

    connect(titleBarWidget_.getCloseButton(),
            SIGNAL(clicked()),
            this,
            SLOT(close()));
    connect(titleBarWidget_.getFloatingButton(),
            SIGNAL(clicked()),
            this,
            SLOT(manageFloating()));

    setTitleBarWidget(&titleBarWidget_);
}

void DockWidget::setNewToolTip(QString text)
{
    setToolTip(text);
}

void DockWidget::manageFloating()
{
    setFloating(!isFloating());
}
