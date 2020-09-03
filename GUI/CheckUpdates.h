#pragma once

#include <QDialog>

namespace Ui
{
class CheckUpdates;
}  // namespace Ui

/**
 * @brief Dialog class with question about auto updating.
 */
class CheckUpdates : public QDialog
{
    Q_OBJECT
public:
    explicit CheckUpdates(QWidget* parent = nullptr);

    ~CheckUpdates() override;

    bool isSaveFlagSet();

private:
    Ui::CheckUpdates* ui;
};
