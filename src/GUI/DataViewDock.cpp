#include "DataViewDock.h"

#include <eible/ExportDsv.h>
#include <QApplication>
#include <QBuffer>
#include <QClipboard>

#include "ModelsAndViews/DataView.h"

DataViewDock::DataViewDock(const QString& title, QWidget* parent)
    : Dock(title, parent)
{
    titleBar_.setButtonVisible(DockTitleBar::Button::EXPORT, true);
    titleBar_.setButtonVisible(DockTitleBar::Button::SELECT_ALL, true);
    titleBar_.setButtonVisible(DockTitleBar::Button::UNSELECT_ALL, true);

    connect(&titleBar_, &DockTitleBar::exportClicked, this,
            &DataViewDock::quickExportData);

    connect(&titleBar_, &DockTitleBar::selectAllClicked, this,
            &DataViewDock::selectAll);

    connect(&titleBar_, &DockTitleBar::unselectAllClicked, this,
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
    titleBar_.setButtonEnabled(DockTitleBar::Button::SELECT_ALL, true);
    titleBar_.setButtonEnabled(DockTitleBar::Button::UNSELECT_ALL, true);
}
