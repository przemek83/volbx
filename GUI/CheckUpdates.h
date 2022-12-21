#pragma once

#include <memory>

#include <QDialog>

#include "ui_CheckUpdates.h"

/**
 * @brief Dialog class with question about auto updating.
 */
class CheckUpdates : public QDialog
{
    Q_OBJECT
public:
    explicit CheckUpdates(QWidget* parent = nullptr);

    bool isSaveFlagSet();

private:
    std::unique_ptr<Ui::CheckUpdates> ui_;
};
