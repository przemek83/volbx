#include "CheckUpdates.h"

#include <QStyle>

CheckUpdates::CheckUpdates(QWidget* parent)
    : QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    ui_->setupUi(this);

    const QStyle* style{QApplication::style()};
    setWindowIcon(style->standardIcon(QStyle::QStyle::SP_MessageBoxQuestion));
}

bool CheckUpdates::isSaveFlagSet() const
{
    return ui_->saveCheckBox->isChecked();
}
