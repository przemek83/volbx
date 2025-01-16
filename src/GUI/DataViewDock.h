#pragma once

#include "Dock.h"

class DataViewDock : public Dock
{
    Q_OBJECT
public:
    DataViewDock(const QString& title, QWidget* parent);

    void activateSelectButtons() const;

private slots:
    void quickExportData() const;

    void selectAll() const;

    void unselectAll() const;
};
