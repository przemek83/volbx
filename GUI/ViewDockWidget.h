#pragma once

#include "Dock.h"

class QPushButton;

/**
 * @brief dock widget for data view.
 */
class ViewDockWidget : public Dock
{
    Q_OBJECT
public:
    explicit ViewDockWidget(const QString& title, QWidget* parent = nullptr,
                            Qt::WindowFlags flags = Qt::Widget);

    ~ViewDockWidget() override = default;

    void activateSelectButtons();

    void deactivateSelectButtons();

private Q_SLOTS:
    void quickExportData();

    void selectAll();

    void unselectAll();
};
