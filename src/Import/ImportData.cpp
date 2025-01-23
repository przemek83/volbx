#include "ImportData.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include <Datasets/Dataset.h>

ImportData::ImportData(const QString& importFilePath)
    : spreadsheetsTab_{importFilePath}
{
    setupLayout();
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

std::unique_ptr<Dataset> ImportData::getSelectedDataset()
{
    const auto* tabWidget{findChild<QTabWidget*>()};
    auto* tab{dynamic_cast<ImportTab*>(tabWidget->currentWidget())};
    return tab->getDataset();
}

QString ImportData::getImportFilePath() const
{
    return spreadsheetsTab_.getCurrentImportFilePath();
}

QDialogButtonBox* ImportData::createButtonBox()
{
    auto* buttonBox{
        new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Cancel,
                             Qt::Horizontal, this)};
    buttonBox->button(QDialogButtonBox::Open)->setEnabled(false);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    return buttonBox;
}

void ImportData::setupLayout()
{
    auto* layout{new QVBoxLayout(this)};

    const int spacing{2};
    layout->setSpacing(spacing);
    const int margin{2};
    layout->setContentsMargins(margin, margin, margin, margin);

    QDialogButtonBox* buttonBox{createButtonBox()};
    auto enableOpenButton{[box = buttonBox](bool activate) {
        box->button(QDialogButtonBox::Open)->setEnabled(activate);
    }};

    setupTabWidget(enableOpenButton);

    layout->addWidget(&tabWidget_);
    layout->addWidget(buttonBox);
}

void ImportData::setupTabWidget(
    const std::function<void(bool)>& enableOpenButton)
{
    connect(&datasetsTab_, &ImportTab::datasetIsReady, enableOpenButton);
    tabWidget_.addTab(&datasetsTab_, tr("Datasets"));

    connect(&spreadsheetsTab_, &ImportTab::datasetIsReady, enableOpenButton);
    tabWidget_.addTab(&spreadsheetsTab_, tr("Spreadsheets"));

    if (datasetsTab_.datasetsAreAvailable())
        tabWidget_.setCurrentWidget(&datasetsTab_);
    else
        tabWidget_.setCurrentWidget(&spreadsheetsTab_);
}
