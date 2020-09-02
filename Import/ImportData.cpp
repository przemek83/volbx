#include "ImportData.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include <Datasets/Dataset.h>

#include "DatasetImportTab.h"
#include "SpreadsheetsImportTab.h"

ImportData::ImportData(QWidget* parent) : QDialog(parent)
{
    setupLayout();
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

std::unique_ptr<Dataset> ImportData::getSelectedDataset()
{
    auto tabWidget{findChild<QTabWidget*>()};
    auto tab{dynamic_cast<ImportTab*>(tabWidget->currentWidget())};
    return tab->getDataset();
}

QDialogButtonBox* ImportData::createButtonBox()
{
    auto buttonBox{
        new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Cancel,
                             Qt::Horizontal, this)};
    buttonBox->button(QDialogButtonBox::Open)->setEnabled(false);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    return buttonBox;
}

void ImportData::setupLayout()
{
    auto layout = new QVBoxLayout(this);
    layout->setSpacing(2);
    layout->setContentsMargins(2, 2, 2, 2);

    QDialogButtonBox* buttonBox{createButtonBox()};
    auto enableOpenButton = [=](bool activate) {
        buttonBox->button(QDialogButtonBox::Open)->setEnabled(activate);
    };
    QTabWidget* tabWidget{createTabWidgetWithContent(enableOpenButton)};

    layout->addWidget(tabWidget);
    layout->addWidget(buttonBox);
}

QTabWidget* ImportData::createTabWidgetWithContent(
    std::function<void(bool)> enableOpenButton)
{
    auto tabWidget{new QTabWidget(this)};

    auto datasetsTab{new DatasetImportTab(tabWidget)};
    connect(datasetsTab, &ImportTab::datasetIsReady, enableOpenButton);
    tabWidget->addTab(datasetsTab, tr("Datasets"));

    auto spreadsheetsTab{new SpreadsheetsImportTab(tabWidget)};
    connect(spreadsheetsTab, &ImportTab::datasetIsReady, enableOpenButton);
    tabWidget->addTab(spreadsheetsTab, tr("Spreadsheets"));

    // If no datasets, than switch to spreadsheets tab.
    if (datasetsTab->datasetsAreAvailable())
        tabWidget->setCurrentWidget(datasetsTab);
    else
        tabWidget->setCurrentWidget(spreadsheetsTab);

    return tabWidget;
}
