#pragma once

#include "Dock.h"

/**
 * @brief Dock for data view.
 */
class DataViewDock : public Dock
{
    Q_OBJECT
public:
    explicit DataViewDock(const QString& title, QWidget* parent = nullptr,
                          Qt::WindowFlags flags = Qt::Widget);

    ~DataViewDock() override = default;

    void activateSelectButtons();

    void deactivateSelectButtons();

private Q_SLOTS:
    void quickExportData();

    void selectAll();

    void unselectAll();
};
