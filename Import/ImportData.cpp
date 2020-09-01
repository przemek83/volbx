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
    auto verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(2);
    verticalLayout->setContentsMargins(2, 2, 2, 2);
    auto tabWidget = new QTabWidget(this);
    verticalLayout->addWidget(tabWidget);

    auto buttonBox{
        new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Cancel,
                             Qt::Horizontal, this)};

    auto enableOpenButton = [=](bool activate) {
        buttonBox->button(QDialogButtonBox::Open)->setEnabled(activate);
    };

    // Create tabs.
    auto datasetsTab{new DatasetImportTab(tabWidget)};
    connect(datasetsTab, &ImportTab::definitionIsReady, enableOpenButton);
    tabWidget->addTab(datasetsTab, tr("Datasets"));

    auto spreadsheetsTab{new SpreadsheetsImportTab(tabWidget)};
    connect(spreadsheetsTab, &ImportTab::definitionIsReady, enableOpenButton);
    tabWidget->addTab(spreadsheetsTab, tr("Spreadsheets"));

    // If no datasets, than switch to spreadsheets tab.
    if (datasetsTab->datasetsAreAvailable())
        tabWidget->setCurrentWidget(datasetsTab);
    else
        tabWidget->setCurrentWidget(spreadsheetsTab);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    verticalLayout->addWidget(buttonBox);

    buttonBox->button(QDialogButtonBox::Open)->setEnabled(false);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

std::unique_ptr<Dataset> ImportData::getSelectedDataset()
{
    auto tabWidget{findChild<QTabWidget*>()};
    auto tab{dynamic_cast<ImportTab*>(tabWidget->currentWidget())};
    return tab->getDataset();
}
