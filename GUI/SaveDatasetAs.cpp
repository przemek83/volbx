#include "SaveDatasetAs.h"

#include <QMessageBox>
#include <QRegExpValidator>

#include <Common/DatasetUtilities.h>

#include "ui_SaveDatasetAs.h"

SaveDatasetAs::SaveDatasetAs(QStringList usedNames, QWidget* parent)
    : QDialog(parent),
      ui(new Ui::SaveDatasetAs),
      usedNames_(std::move(usedNames))
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
    const bool validName{!usedNames_.contains(actualText, Qt::CaseInsensitive)};
    const QPalette::ColorRole colorRole{ui->name->backgroundRole()};

    if (validName && palette.color(colorRole) == QColor(Qt::red))
    {
        palette.setColor(colorRole, Qt::white);
        ui->name->setPalette(palette);
    }

    if (!validName && palette.color(colorRole) != QColor(Qt::red))
    {
        palette.setColor(colorRole, Qt::red);
        ui->name->setPalette(palette);
    }
}

void SaveDatasetAs::saveClicked()
{
    const QString name{ui->name->text()};
    if (usedNames_.contains(name, Qt::CaseInsensitive))
    {
        const QString title{QObject::tr("Overwrite dataset?")};
        const QString msg(QObject::tr("Dataset named ") + name +
                          QObject::tr(" exist. Overwrite?"));
        QMessageBox::StandardButton decision{QMessageBox::question(
            this, title, msg, QMessageBox::Yes | QMessageBox::No)};
        if (decision == QMessageBox::Yes)
            accept();
        else
            return;
    }
    accept();
}

void SaveDatasetAs::cancelClicked() { reject(); }
