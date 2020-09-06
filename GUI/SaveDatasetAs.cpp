#include "SaveDatasetAs.h"

#include <QMessageBox>
#include <QRegExpValidator>

#include <Common/DatasetUtilities.h>

#include "ui_SaveDatasetAs.h"

SaveDatasetAs::SaveDatasetAs(QStringList alreadyUsedNames, QWidget* parent)
    : QDialog(parent),
      ui(new Ui::SaveDatasetAs),
      alreadyUsedNames_(std::move(alreadyUsedNames))
{
    ui->setupUi(this);

    connect(ui->name, &QLineEdit::textChanged, this,
            &SaveDatasetAs::nameChanged);
    const QRegExp datasetNameRegExp(DatasetUtilities::getDatasetNameRegExp());
    ui->name->setValidator(new QRegExpValidator(datasetNameRegExp, this));

    connect(ui->save, &QPushButton::clicked, this, &SaveDatasetAs::saveClicked);
    ui->save->setEnabled(false);

    connect(ui->cancel, &QPushButton::clicked, this,
            &SaveDatasetAs::cancelClicked);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

SaveDatasetAs::~SaveDatasetAs() { delete ui; }

QString SaveDatasetAs::getDatasetName() { return ui->name->text(); }

void SaveDatasetAs::nameChanged(const QString& actualText)
{
    ui->save->setDisabled(actualText.isEmpty());

    QPalette palette{ui->name->palette()};
    if (alreadyUsedNames_.contains(actualText, Qt::CaseInsensitive))
    {
        if (QColor(Qt::red) != palette.color(ui->name->backgroundRole()))
        {
            palette.setColor(ui->name->backgroundRole(), Qt::red);
            ui->name->setPalette(palette);
        }
    }
    else
    {
        if (QColor(Qt::red) == palette.color(ui->name->backgroundRole()))
        {
            palette.setColor(ui->name->backgroundRole(), Qt::white);
            ui->name->setPalette(palette);
        }
    }
}

void SaveDatasetAs::saveClicked()
{
    if (QColor(Qt::red) ==
        ui->name->palette().color(ui->name->backgroundRole()))
    {
        QMessageBox::StandardButton decision;
        QString msg(QObject::tr("Dataset named "));
        msg.append(ui->name->text());
        msg.append(QObject::tr(" exist. Overwrite?"));
        decision =
            QMessageBox::question(this, QObject::tr("Overwrite dataset?"), msg,
                                  QMessageBox::Yes | QMessageBox::No);
        if (decision == QMessageBox::Yes)
            accept();
        else
            return;
    }
    accept();
}

void SaveDatasetAs::cancelClicked() { reject(); }
