#pragma once

#include <memory>

#include <QDialog>

#include "ui_CheckUpdates.h"

class CheckUpdates : public QDialog
{
    Q_OBJECT
public:
    explicit CheckUpdates(QWidget* parent = nullptr);

    bool isSaveFlagSet();

private:
    std::unique_ptr<Ui::CheckUpdates> ui_;
};
