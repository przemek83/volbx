#include "About.h"

#include "Common/Constants.h"

#include "ui_About.h"

About::About(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    ui->versionNumber->setText(QApplication::applicationVersion());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

About::~About()
{
    delete ui;
}
