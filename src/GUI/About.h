#pragma once

#include <memory>

#include <QDialog>

#include "ui_About.h"

class About : public QDialog
{
    Q_OBJECT
public:
    explicit About(QWidget* parent);

private:
    std::unique_ptr<Ui::About> ui_;
};
