#include "CheckUpdates.h"

#include <QStyle>

#include "ui_CheckUpdates.h"

CheckUpdates::CheckUpdates(QWidget* parent)
    : QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint),
      ui(new Ui::CheckUpdates)
{
    ui->setupUi(this);

    const QStyle* style{QApplication::style()};
    setWindowIcon(style->standardIcon(QStyle::QStyle::SP_MessageBoxQuestion));
}

CheckUpdates::~CheckUpdates() { delete ui; }

bool CheckUpdates::isSaveFlagSet() { return ui->saveCheckBox->isChecked(); }
