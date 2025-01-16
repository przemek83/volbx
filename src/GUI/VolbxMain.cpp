#include "VolbxMain.h"

#include <wble/ProgressBarCounter.h>
#include <QActionGroup>
#include <QDir>
#include <QElapsedTimer>
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
#include <ModelsAndViews/FilteringProxyModel.h>
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

VolbxMain::VolbxMain()
    : QMainWindow(),
      ui_(std::make_unique<Ui::VolbxMain>()),
      filters_(this),
      tabWidget_(this)
{
    ui_->setupUi(this);

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

void VolbxMain::setStandardIcons()
{
    const QStyle* style{QApplication::style()};
    ui_->actionImportData->setIcon(
        style->standardIcon(QStyle::SP_DialogOpenButton));
    ui_->actionSaveDatasetAs->setIcon(
        style->standardIcon(QStyle::SP_DialogSaveButton));
    ui_->actionExit->setIcon(style->standardIcon(QStyle::SP_DialogCloseButton));
    ui_->actionLogs->setIcon(
        style->standardIcon(QStyle::SP_FileDialogContentsView));
    ui_->actionCheckForNewVersion->setIcon(
        style->standardIcon(QStyle::SP_BrowserReload));
    ui_->actionAbout->setIcon(
        style->standardIcon(QStyle::QStyle::SP_FileDialogInfoView));
}

void VolbxMain::connectFilter() const
{
    connect(&filters_, &FiltersDock::filterNames, &tabWidget_,
            &TabWidget::setTextFilter);
    connect(&filters_, &FiltersDock::filterDates, &tabWidget_,
            &TabWidget::setDateFilter);
    connect(&filters_, &FiltersDock::filterNumbers, &tabWidget_,
            &TabWidget::setNumericFilter);
}

void VolbxMain::connectPlots() const
{
    connect(ui_->actionBasic_plot, &QAction::triggered, &tabWidget_,
            &TabWidget::addBasicPlot);
    connect(ui_->actionHistogram, &QAction::triggered, &tabWidget_,
            &TabWidget::addHistogramPlot);
    connect(ui_->actionGroup_plot, &QAction::triggered, &tabWidget_,
            &TabWidget::addGroupingPlot);
}

void VolbxMain::connectActions()
{
    connect(ui_->actionExit, &QAction::triggered, this, &VolbxMain::close);
    connect(ui_->actionFilters, &QAction::triggered, this,
            [this]() { filters_.setVisible(!filters_.isVisible()); });
    connect(ui_->actionLogs, &QAction::triggered, this,
            []() { Logger::getInstance().toggleVisibility(); });
    connect(ui_->actionAbout, &QAction::triggered, this,
            &VolbxMain::actionAboutTriggered);
    connect(ui_->actionExport, &QAction::triggered, this,
            &VolbxMain::actionExportTriggered);
    connect(ui_->actionSaveDatasetAs, &QAction::triggered, this,
            &VolbxMain::actionSaveDatasetAsTriggered);
    connect(ui_->actionImportData, &QAction::triggered, this,
            &VolbxMain::actionImportDataTriggered);
    connect(ui_->actionCheckForNewVersion, &QAction::triggered, this,
            &VolbxMain::actionCheckForUpdateTriggered);
    connect(ui_->actionUpdateAuto, &QAction::triggered, this,
            &VolbxMain::actionUpdateAutoToggled);
}

void VolbxMain::setupTabWidget()
{
    ui_->verticalLayout->addWidget(&tabWidget_);

    connect(&tabWidget_, &TabWidget::currentChanged, this,
            &VolbxMain::tabWasChanged);
    connect(&tabWidget_, &TabWidget::tabCloseRequested, this,
            &VolbxMain::closeTab);
}

void VolbxMain::setupFilters()
{
    addDockWidget(Qt::LeftDockWidgetArea, &filters_);
    const int defaultFilterWidth{200};
    filters_.titleBarWidget()->resize(defaultFilterWidth,
                                      filters_.titleBarWidget()->height());
}

void VolbxMain::setupNetworkManager() const
{
    connect(&networkManager_, &QNetworkAccessManager::finished, this,
            &VolbxMain::updateCheckReplyFinished);
}

void VolbxMain::addStyleToMenu(const QString& name,
                               QActionGroup* actionsGroup) const
{
    const QString activeStyle{Configuration::getInstance().getStyleName()};
    auto* action{new QAction(name, actionsGroup)};
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
    ui_->menuOptions->addSection(tr("Updates"));
    ui_->menuOptions->addAction(ui_->actionCheckForNewVersion);
    ui_->menuOptions->addAction(ui_->actionUpdateAuto);
}

void VolbxMain::addStylesSectionToMenu()
{
    ui_->menuOptions->addSection(tr("Styles"));

    auto* actionsGroup{new QActionGroup(this)};

    addStyleToMenu(QStringLiteral("Dark Orange"), actionsGroup);
    addStyleToMenu(QStringLiteral("Rounded Blue"), actionsGroup);
    addStylesFoundInAppDir(actionsGroup);
    addStandardQtStyles(actionsGroup);

    ui_->menuOptions->addActions(actionsGroup->actions());
}

void VolbxMain::addStylesFoundInAppDir(QActionGroup* actionsGroup) const
{
    const QStringList nameFilter(QStringLiteral("*.css"));
    const QDir directory(QCoreApplication::applicationDirPath());
    const QFileInfoList styleFiles{directory.entryInfoList(nameFilter)};
    for (const QFileInfo& styleFile : styleFiles)
        addStyleToMenu(styleFile.baseName(), actionsGroup);
}

void VolbxMain::addStandardQtStyles(QActionGroup* actionsGroup) const
{
    const QStringList qtStylesList{QStyleFactory::keys()};
    for (const QString& style : qtStylesList)
        addStyleToMenu(style, actionsGroup);
}

bool VolbxMain::doesUserWantsToCheckForUpdates()
{
    bool shouldCheckForUpdates{false};
    CheckUpdates dialog(this);

    if (dialog.exec() == QDialog::Accepted)
        shouldCheckForUpdates = true;

    if (dialog.isSaveFlagSet())
        Configuration::getInstance().setUpdatePolicy(shouldCheckForUpdates);

    return shouldCheckForUpdates;
}

void VolbxMain::checkForUpdates()
{
    bool checkForUpdates{false};
    if (!Configuration::getInstance().isUpdatePolicyPicked() &&
        doesUserWantsToCheckForUpdates())
        checkForUpdates = true;
    else
        checkForUpdates = Configuration::getInstance().needToCheckForUpdates();

    if (checkForUpdates)
        actionCheckForUpdateTriggered();

    ui_->actionUpdateAuto->setChecked(
        Configuration::getInstance().needToCheckForUpdates());
}

void VolbxMain::tabWasChanged(int index)
{
    if (index != -1)
        filters_.showFiltersForModel(tabWidget_.getCurrentProxyModel());
    manageActions(index != -1);
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
    std::unique_ptr<QWidget> tabToDelete{tabWidget_.widget(tab)};
    const auto* proxyModel{tabToDelete->findChild<FilteringProxyModel*>()};
    filters_.removeFiltersForModel(proxyModel);
    tabWidget_.removeTab(tab);

    manageActions(tabWidget_.count() != 0);
}

void VolbxMain::actionExportTriggered()
{
    Export exportDialog(tabWidget_.currentWidget(), this);
    exportDialog.exec();
}

void VolbxMain::setTooltipsForChartsActions(bool chartsActive)
{
    if (!chartsActive)
    {
        const QString cannotCreateCharts =
            tr("Time and examined variable columns are not specified.");
        ui_->actionBasic_plot->setToolTip(cannotCreateCharts);
        ui_->actionHistogram->setToolTip(cannotCreateCharts);
        ui_->actionGroup_plot->setToolTip(cannotCreateCharts);
    }
    else
    {
        ui_->actionBasic_plot->setToolTip(ui_->actionBasic_plot->text());
        ui_->actionHistogram->setToolTip(ui_->actionHistogram->text());
        ui_->actionGroup_plot->setToolTip(ui_->actionGroup_plot->text());
    }
}

void VolbxMain::manageActions(bool tabExists)
{
    ui_->actionExport->setEnabled(tabExists);
    ui_->actionSaveDatasetAs->setEnabled(tabExists);

    const bool activateCharts{
        tabExists && tabWidget_.getCurrentDataModel()->areTaggedColumnsSet()};
    ui_->actionBasic_plot->setEnabled(activateCharts);
    ui_->actionHistogram->setEnabled(activateCharts);
    ui_->actionGroup_plot->setEnabled(activateCharts);
    setTooltipsForChartsActions(activateCharts);
}

void VolbxMain::saveDataset(const QString& datasetName) const
{
    const DataView* view{tabWidget_.getCurrentDataView()};
    if (view == nullptr)
        return;

    const QString barTitle{
        constants::getProgressBarTitle(constants::BarTitle::SAVING)};
    ProgressBarCounter bar(barTitle, constants::getProgressBarFullCounter(),
                           nullptr);
    bar.showDetached();

    LOG(LogTypes::IMPORT_EXPORT, "Saving dataset " + datasetName);
    const QString filePath{dataset_utilities::getDatasetsDir() + datasetName +
                           dataset_utilities::getDatasetExtension()};

    QElapsedTimer performanceTimer;
    performanceTimer.start();

    QFile file(filePath);
    ExportVbx exportVbx;
    connect(&exportVbx, &ExportData::progressPercentChanged, &bar,
            &ProgressBarCounter::updateProgress);
    if (exportVbx.generateVbx(*view, file))
        LOG(LogTypes::IMPORT_EXPORT,
            "File saved in " +
                constants::timeFromTimeToSeconds(performanceTimer) +
                " seconds.");
    else
        LOG(LogTypes::IMPORT_EXPORT, QStringLiteral("Saving failed."));
}

void VolbxMain::actionSaveDatasetAsTriggered()
{
    if (!dataset_utilities::isDatasetDirUsable())
    {
        QString msg(tr("Can not access folder "));
        msg.append(dataset_utilities::getDatasetsDir());
        msg.append(tr(" needed for saving dataset."));
        QMessageBox::critical(this, tr("Access denied"), msg);
        return;
    }

    SaveDatasetAs saveAs(dataset_utilities::getListOfAvailableDatasets());
    if (saveAs.exec() == QDialog::Accepted)
        saveDataset(saveAs.getDatasetName());
}

void VolbxMain::importDataset(std::unique_ptr<Dataset> dataset)
{
    if (dataset == nullptr || !dataset->isValid())
    {
        QMessageBox::critical(
            this, tr("Import error"),
            tr("Import error encountered: ") + (dataset == nullptr
                                                    ? tr("dataset is null")
                                                    : dataset->getLastError()));
        return;
    }

    const QString barTitle{
        constants::getProgressBarTitle(constants::BarTitle::LOADING)};
    ProgressBarCounter bar(barTitle, constants::getProgressBarFullCounter(),
                           nullptr);
    QObject::connect(dataset.get(), &Dataset::loadingPercentChanged, &bar,
                     &ProgressBarCounter::updateProgress);
    bar.showDetached();

    QApplication::processEvents();

    QElapsedTimer performanceTimer;
    performanceTimer.start();

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
            constants::timeFromTimeToSeconds(performanceTimer) + " seconds.");

    addMainTabForDataset(std::move(dataset));
}

void VolbxMain::actionImportDataTriggered()
{
    ImportData importData(this);
    if (importData.exec() == QDialog::Accepted)
        importDataset(importData.getSelectedDataset());
}

QString VolbxMain::createNameForTab(const std::unique_ptr<Dataset>& dataset)
{
    QString nameForTabBar{dataset->getName()};
    if (auto [ok, column]{dataset->getTaggedColumn(ColumnTag::VALUE)}; ok)
        nameForTabBar.append(" (" + dataset->getHeaderName(column) + ")");
    return nameForTabBar;
}

void VolbxMain::addMainTabForDataset(std::unique_ptr<Dataset> dataset)
{
    const QString nameForTabBar{createNameForTab(dataset)};
    const QString datasetName{dataset->getName()};

    auto* mainTab{new Tab(std::move(dataset), &tabWidget_)};
    filters_.addFiltersForModel(mainTab->getCurrentProxyModel());
    const int newTabIndex{tabWidget_.addTab(mainTab, nameForTabBar)};
    tabWidget_.setCurrentIndex(newTabIndex);

    manageActions(true);

    ui_->statusBar->showMessage(datasetName + " " + tr("loaded"));
}

void VolbxMain::setupStatusBar()
{
    ui_->statusBar->showMessage(tr("Ready") + "...");
}

bool VolbxMain::canUpdate(QNetworkReply* reply)
{
    if (!networking::replyIsValid(reply))
    {
        ui_->statusBar->showMessage(tr("Connection error encountered."));
        return false;
    }

    auto [newestVersion, notNeededHereList] =
        networking::getAvailableVersionAndFiles(reply);
    if (newestVersion.isEmpty())
    {
        ui_->statusBar->showMessage(tr("Wrong answer received from server."));
        return false;
    }

    if (QApplication::applicationVersion() == newestVersion)
    {
        ui_->statusBar->showMessage(tr("No updates available"));
        return false;
    }

    return true;
}

void VolbxMain::updateApplication()
{
    if (QFile::exists(QCoreApplication::applicationDirPath() + '/' +
                      constants::getUpdaterName() +
                      constants::getExeFileSuffix()))
    {
        QProcess::startDetached(constants::getUpdaterName(), {});
        QCoreApplication::quit();
    }
    else
    {
        QMessageBox::critical(
            nullptr, tr("Wrong installation"),
            tr("Installation is corrupted. Could not find file ") +
                constants::getUpdaterName() + constants::getExeFileSuffix() +
                ".\n" + tr("Can not use update functionality."));
    }
}

void VolbxMain::updateCheckReplyFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    if (!canUpdate(reply))
        return;

    const QMessageBox::StandardButton answer{QMessageBox::question(
        nullptr, tr("New version"),
        tr("New version is available. Download and install it now?"))};

    if (QMessageBox::Yes == answer)
        updateApplication();
}

void VolbxMain::actionCheckForUpdateTriggered()
{
    networkManager_.get(networking::getCurrentVersionRequest());
}

void VolbxMain::actionUpdateAutoToggled(bool alwaysCheck)
{
    Configuration::getInstance().setUpdatePolicy(alwaysCheck);
}

void VolbxMain::styleChanged() const
{
    const auto* action{::qobject_cast<QAction*>(sender())};
    const QString style{action->text()};
    if (QStyleFactory::keys().contains(style))
        application::setQtStyle(style);
    else
        application::setCssStyle(style);
    Configuration::getInstance().setStyleName(style);
}
