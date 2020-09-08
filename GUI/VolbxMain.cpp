#include "VolbxMain.h"

#include <ProgressBarCounter.h>
#include <QDir>
#include <QMessageBox>
#include <QNetworkReply>
#include <QProcess>
#include <QStyle>
#include <QStyleFactory>

#include <Common/Configuration.h>
#include <Common/Constants.h>
#include <Common/DatasetUtilities.h>
#include <Export/ExportVbx.h>
#include <Import/ImportData.h>
#include <ModelsAndViews/TableModel.h>
#include <Shared/Application.h>
#include <Shared/Logger.h>
#include <Shared/Networking.h>

#include "About.h"
#include "CheckUpdates.h"
#include "DataView.h"
#include "Export.h"
#include "FiltersDock.h"
#include "SaveDatasetAs.h"
#include "Tab.h"
#include "TabWidget.h"
#include "ui_VolbxMain.h"

VolbxMain::VolbxMain(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::VolbxMain),
      filters_(new FiltersDock(this)),
      tabWidget_(new TabWidget(this))
{
    ui->setupUi(this);

    setStandardIcons();
    setupStatusBar();
    connectFilter();
    connectPlots();
    setupFilters();
    setupTabWidget();
    setupNetworkManager();
    connectActions();
    createOptionsMenu();
}

VolbxMain::~VolbxMain()
{
    delete ui;
    delete filters_;
}

void VolbxMain::setStandardIcons()
{
    QStyle* style{QApplication::style()};
    ui->actionImportData->setIcon(
        style->standardIcon(QStyle::SP_DialogOpenButton));
    ui->actionSaveDatasetAs->setIcon(
        style->standardIcon(QStyle::SP_DialogSaveButton));
    ui->actionExit->setIcon(style->standardIcon(QStyle::SP_DialogCloseButton));
    ui->actionLogs->setIcon(
        style->standardIcon(QStyle::SP_FileDialogContentsView));
    ui->actionCheckForNewVersion->setIcon(
        style->standardIcon(QStyle::SP_BrowserReload));
    ui->actionAbout->setIcon(
        style->standardIcon(QStyle::QStyle::SP_FileDialogInfoView));
}

void VolbxMain::connectFilter()
{
    connect(filters_, &FiltersDock::filterNames, tabWidget_,
            &TabWidget::setTextFilter);
    connect(filters_, &FiltersDock::filterDates, tabWidget_,
            &TabWidget::setDateFilter);
    connect(filters_, &FiltersDock::filterNumbers, tabWidget_,
            &TabWidget::setNumericFilter);
}

void VolbxMain::connectPlots()
{
    connect(ui->actionBasic_plot, &QAction::triggered, tabWidget_,
            &TabWidget::addBasicPlot);
    connect(ui->actionHistogram, &QAction::triggered, tabWidget_,
            &TabWidget::addHistogramPlot);
    connect(ui->actionGroup_plot, &QAction::triggered, tabWidget_,
            &TabWidget::addGroupingPlot);
}

void VolbxMain::connectActions()
{
    connect(ui->actionExit, &QAction::triggered, this,
            &VolbxMain::actionExitTriggered);
    connect(ui->actionFilters, &QAction::triggered, this,
            &VolbxMain::actionFiltersTriggered);
    connect(ui->actionLogs, &QAction::triggered, this,
            []() { Logger::getInstance().toggleVisibility(); });
    connect(ui->actionAbout, &QAction::triggered, this,
            &VolbxMain::actionAboutTriggered);
    connect(ui->actionExport, &QAction::triggered, this,
            &VolbxMain::actionExportTriggered);
    connect(ui->actionSaveDatasetAs, &QAction::triggered, this,
            &VolbxMain::actionSaveDatasetAsTriggered);
    connect(ui->actionImportData, &QAction::triggered, this,
            &VolbxMain::actionImportDataTriggered);
    connect(ui->actionCheckForNewVersion, &QAction::triggered, this,
            &VolbxMain::actionCheckForNewVersionTriggered);
    connect(ui->actionUpdateAuto, &QAction::triggered, this,
            &VolbxMain::actionUpdateAutoToggled);
}

void VolbxMain::setupTabWidget()
{
    ui->verticalLayout->addWidget(tabWidget_);

    connect(tabWidget_, &TabWidget::currentChanged, this,
            &VolbxMain::tabWasChanged);
    connect(tabWidget_, &TabWidget::tabCloseRequested, this,
            &VolbxMain::closeTab);
}

void VolbxMain::setupFilters()
{
    addDockWidget(Qt::LeftDockWidgetArea, filters_);
    const int defaultFilterWidth{200};
    filters_->titleBarWidget()->resize(defaultFilterWidth,
                                       filters_->titleBarWidget()->height());
}

void VolbxMain::setupNetworkManager()
{
    connect(&networkManager_, &QNetworkAccessManager::finished, this,
            &VolbxMain::updateCheckReplyFinished);
}

void VolbxMain::addStyleToMenu(const QString& name, QActionGroup* actionsGroup)
{
    QString activeStyle{Configuration::getInstance().getStyleName()};
    auto action{new QAction(name, actionsGroup)};
    action->setCheckable(true);
    if (activeStyle == name)
        action->setChecked(true);
    connect(action, &QAction::triggered, this, &VolbxMain::styleChanged);
}

void VolbxMain::createOptionsMenu()
{
    addUpdatesSectionToMenu();
    addStylesSectionToMenu();
}

void VolbxMain::addUpdatesSectionToMenu()
{
    ui->menuOptions->addSection(tr("Updates"));
    ui->menuOptions->addAction(ui->actionCheckForNewVersion);
    ui->menuOptions->addAction(ui->actionUpdateAuto);
}

void VolbxMain::addStylesSectionToMenu()
{
    ui->menuOptions->addSection(tr("Styles"));

    auto actionsGroup{new QActionGroup(this)};

    addStyleToMenu(QStringLiteral("Dark Orange"), actionsGroup);
    addStyleToMenu(QStringLiteral("Rounded Blue"), actionsGroup);
    addStylesFoundInAppDir(actionsGroup);
    addStandardQtStyles(actionsGroup);

    ui->menuOptions->addActions(actionsGroup->actions());
}

void VolbxMain::addStylesFoundInAppDir(QActionGroup* actionsGroup)
{
    QStringList nameFilter(QStringLiteral("*.css"));
    QDir directory(QCoreApplication::applicationDirPath());
    QFileInfoList styleFiles{directory.entryInfoList(nameFilter)};
    for (const QFileInfo& styleFile : styleFiles)
        addStyleToMenu(styleFile.baseName(), actionsGroup);
}

void VolbxMain::addStandardQtStyles(QActionGroup* actionsGroup)
{
    QStringList qtStylesList{QStyleFactory::keys()};
    for (const QString& style : qtStylesList)
        addStyleToMenu(style, actionsGroup);
}

void VolbxMain::checkForUpdates()
{
    bool checkForUpdates{false};
    if (!Configuration::getInstance().isUpdatePolicyPicked())
    {
        CheckUpdates dialog(this);
        int reply{dialog.exec()};
        if (reply == QDialog::Accepted)
            checkForUpdates = true;

        // Remember if choice was checked.
        if (dialog.isSaveFlagSet())
            Configuration::getInstance().setUpdatePolicy(checkForUpdates);
    }
    else
    {
        checkForUpdates = Configuration::getInstance().needToCheckForUpdates();
    }

    if (checkForUpdates)
        actionCheckForNewVersionTriggered();

    ui->actionUpdateAuto->setChecked(
        Configuration::getInstance().needToCheckForUpdates());
}

void VolbxMain::actionExitTriggered() { close(); }

void VolbxMain::actionFiltersTriggered()
{
    filters_->setVisible(!filters_->isVisible());
}

void VolbxMain::tabWasChanged(int index)
{
    if (-1 != index)
    {
        const FilteringProxyModel* model = tabWidget_->getCurrentProxyModel();
        filters_->showFiltersForModel(model);
        manageActions(true);
    }
    else
    {
        manageActions(false);
    }
}

void VolbxMain::closeEvent(QCloseEvent* event)
{
    Configuration::getInstance().save();

    QMainWindow::closeEvent(event);

    // If logger window is shown closing mainWindow do not close app.
    QApplication::closeAllWindows();
}

void VolbxMain::actionAboutTriggered()
{
    About about(this);
    about.exec();
}

void VolbxMain::closeTab(int tab)
{
    QWidget* tabToDelete{tabWidget_->widget(tab)};
    const FilteringProxyModel* model{tabWidget_->getCurrentProxyModel()};
    filters_->removeFiltersForModel(model);
    tabWidget_->removeTab(tab);
    delete tabToDelete;

    manageActions(0 != tabWidget_->count());
}

void VolbxMain::actionExportTriggered()
{
    Export exportDialog(dynamic_cast<QMainWindow*>(tabWidget_->currentWidget()),
                        this);

    exportDialog.exec();
}

void VolbxMain::manageActions(bool tabExists)
{
    ui->actionExport->setEnabled(tabExists);
    ui->actionSaveDatasetAs->setEnabled(tabExists);

    // Check if plot icons should be enabled.
    bool activateCharts = tabExists;
    if (tabExists)
    {
        const TableModel* dataModel = tabWidget_->getCurrentDataModel();
        if (nullptr != dataModel)
        {
            activateCharts =
                (activateCharts && dataModel->areTaggedColumnsSet());
        }
    }

    // Enable/disable plot icons.
    ui->actionBasic_plot->setEnabled(activateCharts);
    ui->actionHistogram->setEnabled(activateCharts);
    ui->actionGroup_plot->setEnabled(activateCharts);

    // Set tooltips for plot icons.
    if (!activateCharts)
    {
        const QString cannotCreateCharts =
            tr("Time and examined variable columns are not specified.");
        ui->actionBasic_plot->setToolTip(cannotCreateCharts);
        ui->actionHistogram->setToolTip(cannotCreateCharts);
        ui->actionGroup_plot->setToolTip(cannotCreateCharts);
    }
    else
    {
        ui->actionBasic_plot->setToolTip(ui->actionBasic_plot->text());
        ui->actionHistogram->setToolTip(ui->actionHistogram->text());
        ui->actionGroup_plot->setToolTip(ui->actionGroup_plot->text());
    }
}

void VolbxMain::actionSaveDatasetAsTriggered()
{
    if (!DatasetUtilities::doesDatasetDirExistAndUserHavePermisions())
    {
        QString msg(tr("Can not access folder "));
        msg.append(DatasetUtilities::getDatasetsDir());
        msg.append(tr(" needed for saving dataset."));
        QMessageBox::critical(this, QString(tr("Access denied")), msg);

        return;
    }

    SaveDatasetAs save(DatasetUtilities::getListOfAvailableDatasets());

    if (QDialog::Accepted == save.exec())
    {
        DataView* view = tabWidget_->getCurrentDataView();
        if (view == nullptr)
            return;

        QString barTitle =
            Constants::getProgressBarTitle(Constants::BarTitle::SAVING);
        ProgressBarCounter bar(barTitle, 100, nullptr);
        bar.showDetached();

        QString name{save.getDatasetName()};
        LOG(LogTypes::IMPORT_EXPORT, "Saving dataset " + name);
        QString filePath{DatasetUtilities::getDatasetsDir() + name +
                         DatasetUtilities::getDatasetExtension()};

        QTime performanceTimer;
        performanceTimer.start();

        QFile file(filePath);
        ExportVbx exportVbx;
        connect(&exportVbx, &ExportData::progressPercentChanged, &bar,
                &ProgressBarCounter::updateProgress);
        if (!exportVbx.generateVbx(*view, file))
            LOG(LogTypes::IMPORT_EXPORT, "Saving failed.");

        LOG(LogTypes::IMPORT_EXPORT,
            "File saved in " +
                QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
                " seconds.");
    }
}

void VolbxMain::actionImportDataTriggered()
{
    ImportData import(this);

    if (QDialog::Accepted != import.exec())
        return;

    std::unique_ptr<Dataset> dataset{import.getSelectedDataset()};

    if (dataset == nullptr || !dataset->isValid())
    {
        QMessageBox::critical(
            this, tr("Import error"),
            tr("Import error encountered, can not continue operation."));
        return;
    }

    if (!dataset)
    {
        QMessageBox::critical(this, tr("Import error"),
                              dataset->getLastError());
        return;
    }

    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::LOADING);
    ProgressBarCounter bar(barTitle, 100, nullptr);
    QObject::connect(dataset.get(), &Dataset::loadingPercentChanged, &bar,
                     &ProgressBarCounter::updateProgress);
    bar.showDetached();

    QApplication::processEvents();

    QTime performanceTimer;
    performanceTimer.start();

    // TODO 26/08/2012 Currently try catches only part of memory problems.
    // One problem still lies in creating 2d array.
    // Try to create vector of pointers to 1d arrays.
    try
    {
        dataset->loadData();
    }
    catch (std::bad_alloc&)
    {
        QString message(tr("Not enough memory to open data. "));
        message.append(tr("Close not needed data,"));
        message.append(
            tr(" pick smaller set or use another instance of application."));
        QMessageBox::critical(this, tr("Memory problem"), message);

        return;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Loaded file having " + QString::number(dataset->rowCount()) +
            " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    addMainTabForDataset(std::move(dataset));
}

void VolbxMain::addMainTabForDataset(std::unique_ptr<Dataset> dataset)
{
    QString nameForTabBar{dataset->getName()};
    auto [exist, column] = dataset->getTaggedColumn(ColumnTag::VALUE);
    if (exist)
        nameForTabBar.append(" (" + dataset->getHeaderName(column) + ")");

    QString datasetName{dataset->getName()};
    auto mainTab = new Tab(std::move(dataset), tabWidget_);
    const FilteringProxyModel* proxyModel = mainTab->getCurrentProxyModel();
    if (nullptr != proxyModel)
    {
        filters_->addFiltersForModel(proxyModel);
    }

    int newTabIndex = tabWidget_->addTab(mainTab, nameForTabBar);
    tabWidget_->setCurrentIndex(newTabIndex);

    manageActions(true);

    ui->statusBar->showMessage(datasetName + " " + tr("loaded"));
}

void VolbxMain::setupStatusBar()
{
    ui->statusBar->showMessage(tr("Ready") + "...");
}

void VolbxMain::updateCheckReplyFinished(QNetworkReply* reply)
{
    reply->deleteLater();

    // Check errors.
    if (!Networking::replyIsValid(reply))
    {
        ui->statusBar->showMessage(tr("Connection error encountered."));
        return;
    }

    auto [newestVersion, notNeededHereList] =
        Networking::getAvailableVersionAndFiles(reply);

    if (newestVersion.isEmpty())
    {
        ui->statusBar->showMessage(tr("Wrong answer received from server."));
        return;
    }

    if (newestVersion != QApplication::applicationVersion())
    {
        QMessageBox::StandardButton answer = QMessageBox::question(
            nullptr, tr("New version"),
            tr("New version is available. Download and install it now?"));

        // When user want to update.
        if (QMessageBox::Yes == answer)
        {
            if (QFile::exists(QCoreApplication::applicationDirPath() + '/' +
                              Constants::getUpdaterName() +
                              Constants::getExeFileSuffix()))
            {
                QProcess::startDetached(Constants::getUpdaterName());
                QCoreApplication::quit();
            }
            else
            {
                QMessageBox::critical(
                    nullptr, tr("Wrong installation"),
                    tr("Installation is corrupted. Could not find file ") +
                        Constants::getUpdaterName() +
                        Constants::getExeFileSuffix() + ".\n" +
                        tr("Can not use update functionality."));
            }
        }
    }
    else
    {
        ui->statusBar->showMessage(tr("No updates available"));
    }
}

void VolbxMain::actionCheckForNewVersionTriggered()
{
    networkManager_.get(Networking::getCurrentVersionRequest());
}

void VolbxMain::actionUpdateAutoToggled(bool alwaysCheck)
{
    Configuration::getInstance().setUpdatePolicy(alwaysCheck);
}

void VolbxMain::styleChanged()
{
    auto action{qobject_cast<QAction*>(sender())};
    QString style{action->text()};
    if (QStyleFactory::keys().contains(style))
        Application::setQtStyle(style);
    else
        Application::setCssStyle(style);
    Configuration::getInstance().setStyleName(style);
}
