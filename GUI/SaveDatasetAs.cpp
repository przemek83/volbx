#include "SaveDatasetAs.h"

#include <QMessageBox>
#include <QRegularExpressionValidator>

#include <Common/DatasetUtilities.h>

SaveDatasetAs::SaveDatasetAs(QStringList usedNames, QWidget* parent)
    : QDialog(parent),
      ui_(std::make_unique<Ui::SaveDatasetAs>()),
      usedNames_(std::move(usedNames))
{
    ui_->setupUi(this);

    connect(ui_->name, &QLineEdit::textChanged, this,
            &SaveDatasetAs::nameChanged);
    const QRegularExpression datasetNameRegExp(
        DatasetUtilities::getDatasetNameRegExp());
    auto* validator{new QRegularExpressionValidator(datasetNameRegExp, this)};
    ui_->name->setValidator(validator);

    connect(ui_->save, &QPushButton::clicked, this,
            &SaveDatasetAs::saveClicked);
    ui_->save->setEnabled(false);

    connect(ui_->cancel, &QPushButton::clicked, this,
            &SaveDatasetAs::cancelClicked);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

QString SaveDatasetAs::getDatasetName() { return ui_->name->text(); }

bool SaveDatasetAs::overwriteDataset(const QString& name)
{
    const QString title{QObject::tr("Overwrite dataset?")};
    const QString msg(QObject::tr("Dataset named ") + name +
                      QObject::tr(" exist. Overwrite?"));
    const QMessageBox::StandardButton decision{QMessageBox::question(
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
    ui_->save->setDisabled(actualText.isEmpty());
    adjustWidgetBackgroundColor(ui_->name, nameIsUsed(actualText));
}

void SaveDatasetAs::saveClicked()
{
    const QString name{ui_->name->text()};
    if (nameIsUsed(name) && !overwriteDataset(name))
        return;
    accept();
}

void SaveDatasetAs::cancelClicked() { reject(); }
