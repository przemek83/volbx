#include <QRegExpValidator>
#include <QDebug>
#include <QPushButton>
#include <QMessageBox>

#include "Common/Constants.h"

#include "SaveDatasetAs.h"
#include "ui_SaveDatasetAs.h"

SaveDatasetAs::SaveDatasetAs(QStringList alreadyUsedNames, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SaveDatasetAs),
    alreadyUsedNames_(std::move(alreadyUsedNames))
{
    ui->setupUi(this);

    ui->nameLineEdit->setValidator(new QRegExpValidator(QRegExp(Constants::datasetNameRegExp_),
                                                        this));
    ui->save->setEnabled(false);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

SaveDatasetAs::~SaveDatasetAs()
{
    delete ui;
}

QString SaveDatasetAs::getChosenDatasetName()
{
    return ui->nameLineEdit->text();
}

void SaveDatasetAs::on_nameLineEdit_textChanged(const QString& actualText)
{
    if (true == actualText.isEmpty())
    {
        ui->save->setEnabled(false);
    }
    else
    {
        ui->save->setEnabled(true);
    }

    QPalette palette = ui->nameLineEdit->palette();
    if (alreadyUsedNames_.contains(actualText, Qt::CaseInsensitive))
    {
        if (QColor(Qt::red) != palette.color(ui->nameLineEdit->backgroundRole()))
        {
            palette.setColor(ui->nameLineEdit->backgroundRole(), Qt::red);
            ui->nameLineEdit->setPalette(palette);
        }
    }
    else
    {
        if (QColor(Qt::red) == palette.color(ui->nameLineEdit->backgroundRole()))
        {
            palette.setColor(ui->nameLineEdit->backgroundRole(), Qt::white);
            ui->nameLineEdit->setPalette(palette);
        }
    }
}

void SaveDatasetAs::on_save_clicked()
{
    if (QColor(Qt::red) == ui->nameLineEdit->palette().color(ui->nameLineEdit->backgroundRole()))
    {
        QMessageBox::StandardButton decision;
        QString msg(QObject::tr("Dataset named "));
        msg.append(ui->nameLineEdit->text());
        msg.append(QObject::tr(" exist. Overwrite?"));
        decision = QMessageBox::question(this,
                                         QObject::tr("Overwrite dataset?"),
                                         msg,
                                         QMessageBox::Yes | QMessageBox::No);
        if (decision == QMessageBox::Yes)
        {
            accept();
        }
        else
        {
            return;
        }
    }
    accept();
}

void SaveDatasetAs::on_cancel_clicked()
{
    reject();
}
