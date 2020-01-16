#include "CheckUpdatesDialog.h"

#include "ui_CheckUpdatesDialog.h"

CheckUpdatesDialog::CheckUpdatesDialog(QWidget* parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint),
    ui(new Ui::CheckUpdatesDialog)
{
    ui->setupUi(this);
}

CheckUpdatesDialog::~CheckUpdatesDialog()
{
    delete ui;
}

bool CheckUpdatesDialog::saveFlagSet()
{
    return ui->saveCheckBox->isChecked();
}
