#pragma once

#include <QDialog>

namespace Ui
{
class About;
}  // namespace Ui

/**
 * @brief The About dialog class.
 */
class About : public QDialog
{
    Q_OBJECT
public:
    explicit About(QWidget* parent = nullptr);

    ~About() override;

private:
    Ui::About* ui;
};
