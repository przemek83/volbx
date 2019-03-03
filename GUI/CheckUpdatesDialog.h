#ifndef CHECKUPDATESDIALOG_H
#define CHECKUPDATESDIALOG_H

#include <QDialog>

namespace Ui {
class CheckUpdatesDialog;
}

/**
 * @brief Dialog class with question about auto updating.
 */
class CheckUpdatesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CheckUpdatesDialog(QWidget *parent = nullptr);

    virtual ~CheckUpdatesDialog();

    bool saveFlagSet();

private:
    Q_DISABLE_COPY(CheckUpdatesDialog)

    Ui::CheckUpdatesDialog *ui;
};

#endif // CHECKUPDATESDIALOG_H
