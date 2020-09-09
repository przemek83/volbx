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
    auto* validator{new QRegExpValidator(datasetNameRegExp, this)};
    ui->name->setValidator(validator);

    connect(ui->save, &QPushButton::clicked, this, &SaveDatasetAs::saveClicked);
    ui->save->setEnabled(false);

    connect(ui->cancel, &QPushButton::clicked, this,
            &SaveDatasetAs::cancelClicked);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

SaveDatasetAs::~SaveDatasetAs() { delete ui; }

QString SaveDatasetAs::getDatasetName() { return ui->name->text(); }

bool SaveDatasetAs::overwriteDataset(const QString& name)
{
    const QString title{QObject::tr("Overwrite dataset?")};
    const QString msg(QObject::tr("Dataset named ") + name +
                      QObject::tr(" exist. Overwrite?"));
    QMessageBox::StandardButton decision{QMessageBox::question(
        this, title, msg, QMessageBox::Yes | QMessageBox::No)};
    return decision == QMessageBox::Yes;
}

bool SaveDatasetAs::nameIsUsed(const QString& name)
{
    return usedNames_.contains(name, Qt::CaseInsensitive);
}

void SaveDatasetAs::adjustWidgetBackgroundColor(QWidget* widget, bool nameUsed)
{
    QPalette palette{widget->palette()};
    const QPalette::ColorRole colorRole{widget->backgroundRole()};

    if (!nameUsed && palette.color(colorRole) == QColor(Qt::red))
    {
        palette.setColor(colorRole, Qt::white);
        widget->setPalette(palette);
    }

    if (nameUsed && palette.color(colorRole) != QColor(Qt::red))
    {
        palette.setColor(colorRole, Qt::red);
        widget->setPalette(palette);
    }
}

void SaveDatasetAs::nameChanged(const QString& actualText)
{
    ui->save->setDisabled(actualText.isEmpty());
    adjustWidgetBackgroundColor(ui->name, nameIsUsed(actualText));
}

void SaveDatasetAs::saveClicked()
{
    const QString name{ui->name->text()};
    if (nameIsUsed(name) && !overwriteDataset(name))
        return;
    accept();
}

void SaveDatasetAs::cancelClicked() { reject(); }
