#include "CheckUpdatesDialog.h"

#include <QStyle>

#include "ui_CheckUpdatesDialog.h"

CheckUpdatesDialog::CheckUpdatesDialog(QWidget* parent)
    : QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint),
      ui(new Ui::CheckUpdatesDialog)
{
    ui->setupUi(this);

    const QStyle* style = QApplication::style();
    setWindowIcon(style->standardIcon(QStyle::QStyle::SP_MessageBoxQuestion));
}

CheckUpdatesDialog::~CheckUpdatesDialog() { delete ui; }

bool CheckUpdatesDialog::isSaveFlagSet()
{
    return ui->saveCheckBox->isChecked();
}
