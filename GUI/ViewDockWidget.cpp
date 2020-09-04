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
    titleBarWidget_.setButtonVisible(DockTitleBar::Button::EXPORT, true);
    titleBarWidget_.setButtonVisible(DockTitleBar::Button::SELECT_ALL, true);
    titleBarWidget_.setButtonVisible(DockTitleBar::Button::UNSELECT_ALL, true);

    connect(&titleBarWidget_, &DockTitleBar::exportClicked, this,
            &ViewDockWidget::quickExportData);

    connect(&titleBarWidget_, &DockTitleBar::selectAllClicked, this,
            &ViewDockWidget::selectAll);

    connect(&titleBarWidget_, &DockTitleBar::unselectAllClicked, this,
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
    view->recomputeAllData();
}

void ViewDockWidget::unselectAll()
{
    auto view = findChild<DataView*>();

    if (view == nullptr)
        return;

    view->clearSelection();
    view->recomputeAllData();
}

void ViewDockWidget::activateSelectButtons()
{
    titleBarWidget_.setButtonEnabled(DockTitleBar::Button::SELECT_ALL, true);
    titleBarWidget_.setButtonEnabled(DockTitleBar::Button::UNSELECT_ALL, true);
}

void ViewDockWidget::deactivateSelectButtons()
{
    titleBarWidget_.setButtonEnabled(DockTitleBar::Button::SELECT_ALL, false);
    titleBarWidget_.setButtonEnabled(DockTitleBar::Button::UNSELECT_ALL, false);
}
