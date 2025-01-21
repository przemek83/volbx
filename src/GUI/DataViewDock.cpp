#include "DataViewDock.h"

#include <eible/ExportDsv.h>
#include <QApplication>
#include <QBuffer>
#include <QClipboard>

#include "DockTitleBar.h"
#include "ModelsAndViews/DataView.h"

DataViewDock::DataViewDock(const QString& title, QWidget* parent)
    : Dock(title, parent)
{
    const DockTitleBar& titleBar{getTitleBar()};
    titleBar.setButtonVisible(DockTitleBar::Button::EXPORT, true);
    titleBar.setButtonVisible(DockTitleBar::Button::SELECT_ALL, true);
    titleBar.setButtonVisible(DockTitleBar::Button::UNSELECT_ALL, true);

    connect(&titleBar, &DockTitleBar::exportClicked, this,
            &DataViewDock::quickExportData);

    connect(&titleBar, &DockTitleBar::selectAllClicked, this,
            &DataViewDock::selectAll);

    connect(&titleBar, &DockTitleBar::unselectAllClicked, this,
            &DataViewDock::unselectAll);
}

void DataViewDock::quickExportData() const
{
    const auto* view{findChild<DataView*>()};
    QByteArray exportedByteArray;
    QBuffer exportedBuffer(&exportedByteArray);
    exportedBuffer.open(QIODevice::WriteOnly);

    ExportDsv exportDsv('\t');
    exportDsv.exportView(*view, exportedBuffer);

    QApplication::clipboard()->setText(QString::fromUtf8(exportedByteArray));
}

void DataViewDock::selectAll() const
{
    auto* view{findChild<DataView*>()};
    view->selectAll();
    view->recomputeAllData();
}

void DataViewDock::unselectAll() const
{
    auto* view{findChild<DataView*>()};
    view->clearSelection();
    view->recomputeAllData();
}

void DataViewDock::activateSelectButtons() const
{
    const DockTitleBar& titleBar{getTitleBar()};
    titleBar.setButtonEnabled(DockTitleBar::Button::SELECT_ALL, true);
    titleBar.setButtonEnabled(DockTitleBar::Button::UNSELECT_ALL, true);
}
