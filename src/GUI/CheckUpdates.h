#pragma once

#include <memory>

#include <QDialog>

#include "ui_CheckUpdates.h"

class CheckUpdates : public QDialog
{
    Q_OBJECT
public:
    explicit CheckUpdates(QWidget* parent);

    bool isSaveFlagSet() const;

private:
    std::unique_ptr<Ui::CheckUpdates> ui_{std::make_unique<Ui::CheckUpdates>()};
};
