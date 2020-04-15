#include "ViewDockWidget.h"

#include <ExportDsv.h>
#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QDebug>
#include <QPushButton>

#include "Common/Constants.h"
#include "ModelsAndViews/DataView.h"

#include "DockTitleBar.h"

ViewDockWidget::ViewDockWidget(const QString& title, QWidget* parent,
                               Qt::WindowFlags flags)
    : DockWidget(title, parent, flags)
{
    titleBarWidget_.getExportButton()->setVisible(true);
    titleBarWidget_.getSelectAllButton()->setVisible(true);
    titleBarWidget_.getUnselectAllButton()->setVisible(true);

    connect(titleBarWidget_.getExportButton(), &QPushButton::clicked, this,
            &ViewDockWidget::quickExportData);

    connect(titleBarWidget_.getSelectAllButton(), &QPushButton::clicked, this,
            &ViewDockWidget::selectAll);

    connect(titleBarWidget_.getUnselectAllButton(), &QPushButton::clicked, this,
            &ViewDockWidget::unselectAll);
}

void ViewDockWidget::quickExportData()
{
    auto view = findChild<DataView*>();
    Q_ASSERT(view != nullptr);

    QByteArray exportedByteArray;
    QBuffer exportedBuffer(&exportedByteArray);
    exportedBuffer.open(QIODevice::WriteOnly);

    ExportDsv exportDsv('\t');
    exportDsv.exportView(*view, exportedBuffer);

    QApplication::clipboard()->setText(QString::fromUtf8(exportedByteArray));
}

void ViewDockWidget::selectAll()
{
    auto view = findChild<DataView*>();

    if (view == nullptr)
        return;

    view->selectAll();
    view->reloadSelectionDataAndRecompute();
}

void ViewDockWidget::unselectAll()
{
    auto view = findChild<DataView*>();

    if (view == nullptr)
        return;

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
