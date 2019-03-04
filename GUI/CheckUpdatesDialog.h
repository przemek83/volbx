#ifndef CHECKUPDATESDIALOG_H
#define CHECKUPDATESDIALOG_H

#include <QDialog>

namespace Ui
{
class CheckUpdatesDialog;
} // namespace Ui

/**
 * @brief Dialog class with question about auto updating.
 */
class CheckUpdatesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CheckUpdatesDialog(QWidget* parent = nullptr);

    ~CheckUpdatesDialog() override;

    CheckUpdatesDialog& operator=(const CheckUpdatesDialog& other) = delete;
    CheckUpdatesDialog(const CheckUpdatesDialog& other) = delete;

    CheckUpdatesDialog& operator=(CheckUpdatesDialog&& other) = delete;
    CheckUpdatesDialog(CheckUpdatesDialog&& other) = delete;

    bool saveFlagSet();

private:
    Ui::CheckUpdatesDialog* ui;
};

#endif // CHECKUPDATESDIALOG_H
