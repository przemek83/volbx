#include <QPushButton>
#include <QDebug>

#include "Common/ExportData.h"
#include "Common/Constants.h"

#include "DockTitleBar.h"
#include "ModelsAndViews/DataView.h"
#include "ViewDockWidget.h"

ViewDockWidget::ViewDockWidget(const QString& title,
                               QWidget* parent,
                               Qt::WindowFlags flags)
    : DockWidget(title, parent, flags)
{
    titleBarWidget_.getExportButton()->setVisible(true);
    titleBarWidget_.getSelectAllButton()->setVisible(true);
    titleBarWidget_.getUnselectAllButton()->setVisible(true);

    connect(titleBarWidget_.getExportButton(),
            SIGNAL(clicked()),
            this,
            SLOT(quickExportData()));
    connect(titleBarWidget_.getSelectAllButton(),
            SIGNAL(clicked()),
            this,
            SLOT(selectAll()));
    connect(titleBarWidget_.getUnselectAllButton(),
            SIGNAL(clicked()),
            this,
            SLOT(unselectAll()));
}

void ViewDockWidget::quickExportData()
{
    auto view = findChild<DataView*>();
    Q_ASSERT(nullptr != view);

    ExportData::quickExportAsTSV(view);
}

void ViewDockWidget::selectAll()
{
    auto view = findChild<DataView*>();

    if (!view)
    {
        return;
    }

    view->selectAll();
    view->reloadSelectionDataAndRecompute();
}

void ViewDockWidget::unselectAll()
{
    auto view = findChild<DataView*>();

    if (!view)
    {
        return;
    }

    view->clearSelection();
    view->reloadSelectionDataAndRecompute();
}

void ViewDockWidget::activateSelectButtons()
{
    titleBarWidget_.getSelectAllButton()->setDisabled(false);
    titleBarWidget_.getUnselectAllButton()->setDisabled(false);
}

void ViewDockWidget::deactivateSelectButtons()
{
    titleBarWidget_.getSelectAllButton()->setDisabled(true);
    titleBarWidget_.getUnselectAllButton()->setDisabled(true);
}
