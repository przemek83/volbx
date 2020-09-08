#include "About.h"

#include <QStyle>

#include "ui_About.h"

About::About(QWidget* parent) : QDialog(parent), ui(new Ui::About)
{
    ui->setupUi(this);

    ui->versionNumber->setText(QApplication::applicationVersion());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    const QStyle* style{QApplication::style()};
    setWindowIcon(style->standardIcon(QStyle::QStyle::SP_FileDialogInfoView));
}

About::~About() { delete ui; }
