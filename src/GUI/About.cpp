#include "About.h"

#include <QStyle>

About::About(QWidget* parent) : QDialog(parent)
{
    ui_->setupUi(this);

    ui_->versionNumber->setText(QApplication::applicationVersion());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    const QStyle* style{QApplication::style()};
    setWindowIcon(style->standardIcon(QStyle::QStyle::SP_FileDialogInfoView));
}
