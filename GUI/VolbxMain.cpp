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
#include "ui_VolbxMain.h"

VolbxMain::VolbxMain(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::VolbxMain),
      filters_(this),
      tabWidget_(this)
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

VolbxMain::~VolbxMain() { delete ui; }

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
    connect(&filters_, &FiltersDock::filterNames, &tabWidget_,
            &TabWidget::setTextFilter);
    connect(&filters_, &FiltersDock::filterDates, &tabWidget_,
            &TabWidget::setDateFilter);
    connect(&filters_, &FiltersDock::filterNumbers, &tabWidget_,
            &TabWidget::setNumericFilter);
}

void VolbxMain::connectPlots()
{
    connect(ui->actionBasic_plot, &QAction::triggered, &tabWidget_,
            &TabWidget::addBasicPlot);
    connect(ui->actionHistogram, &QAction::triggered, &tabWidget_,
            &TabWidget::addHistogramPlot);
    connect(ui->actionGroup_plot, &QAction::triggered, &tabWidget_,
            &TabWidget::addGroupingPlot);
}

void VolbxMain::connectActions()
{
    connect(ui->actionExit, &QAction::triggered, this, &VolbxMain::close);
    connect(ui->actionFilters, &QAction::triggered,
            [&]() { filters_.setVisible(!filters_.isVisible()); });
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
    ui->verticalLayout->addWidget(&tabWidget_);

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

void VolbxMain::setupNetworkManager()
{
    connect(&networkManager_, &QNetworkAccessManager::finished, this,
            &VolbxMain::updateCheckReplyFinished);
}

void VolbxMain::addStyleToMenu(const QString& name,
                               QActionGroup* actionsGroup) const
{
    QString activeStyle{Configuration::getInstance().getStyleName()};
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
    ui->menuOptions->addSection(tr("Updates"));
    ui->menuOptions->addAction(ui->actionCheckForNewVersion);
    ui->menuOptions->addAction(ui->actionUpdateAuto);
}

void VolbxMain::addStylesSectionToMenu()
{
    ui->menuOptions->addSection(tr("Styles"));

    auto* actionsGroup{new QActionGroup(this)};

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

bool VolbxMain::doesUserWantsToCheckForUpdates()
{
    bool checkForUpdates{false};
    CheckUpdates dialog(this);
    int reply{dialog.exec()};
    if (reply == QDialog::Accepted)
        checkForUpdates = true;

    if (dialog.isSaveFlagSet())
        Configuration::getInstance().setUpdatePolicy(checkForUpdates);

    return checkForUpdates;
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
        actionCheckForNewVersionTriggered();

    ui->actionUpdateAuto->setChecked(
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
    QWidget* tabToDelete{tabWidget_.widget(tab)};
    const auto* proxyModel{tabToDelete->findChild<FilteringProxyModel*>()};
    filters_.removeFiltersForModel(proxyModel);
    tabWidget_.removeTab(tab);
    delete tabToDelete;

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

void VolbxMain::manageActions(bool tabExists)
{
    ui->actionExport->setEnabled(tabExists);
    ui->actionSaveDatasetAs->setEnabled(tabExists);

    const bool activateCharts{
        tabExists && tabWidget_.getCurrentDataModel()->areTaggedColumnsSet()};
    ui->actionBasic_plot->setEnabled(activateCharts);
    ui->actionHistogram->setEnabled(activateCharts);
    ui->actionGroup_plot->setEnabled(activateCharts);
    setTooltipsForChartsActions(activateCharts);
}

void VolbxMain::saveDataset(const QString& datasetName)
{
    DataView* view{tabWidget_.getCurrentDataView()};
    if (view == nullptr)
        return;

    QString barTitle{
        Constants::getProgressBarTitle(Constants::BarTitle::SAVING)};
    ProgressBarCounter bar(barTitle, Constants::getProgressBarFullCounter(),
                           nullptr);
    bar.showDetached();

    LOG(LogTypes::IMPORT_EXPORT, "Saving dataset " + datasetName);
    QString filePath{DatasetUtilities::getDatasetsDir() + datasetName +
                     DatasetUtilities::getDatasetExtension()};

    QTime performanceTimer;
    performanceTimer.start();

    QFile file(filePath);
    ExportVbx exportVbx;
    connect(&exportVbx, &ExportData::progressPercentChanged, &bar,
            &ProgressBarCounter::updateProgress);
    if (exportVbx.generateVbx(*view, file))
        LOG(LogTypes::IMPORT_EXPORT,
            "File saved in " +
                Constants::timeFromTimeToSeconds(performanceTimer) +
                " seconds.");
    else
        LOG(LogTypes::IMPORT_EXPORT, "Saving failed.");
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

    SaveDatasetAs saveAs(DatasetUtilities::getListOfAvailableDatasets());
    if (saveAs.exec() == QDialog::Accepted)
        saveDataset(saveAs.getDatasetName());
}

void VolbxMain::importDataset(std::unique_ptr<Dataset> dataset)
{
    if (dataset == nullptr || !dataset->isValid())
    {
        QMessageBox::critical(
            this, tr("Import error"),
            tr("Import error encountered: ") + dataset->getLastError());
        return;
    }

    const QString barTitle{
        Constants::getProgressBarTitle(Constants::BarTitle::LOADING)};
    ProgressBarCounter bar(barTitle, Constants::getProgressBarFullCounter(),
                           nullptr);
    QObject::connect(dataset.get(), &Dataset::loadingPercentChanged, &bar,
                     &ProgressBarCounter::updateProgress);
    bar.showDetached();

    QApplication::processEvents();

    QTime performanceTimer;
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
            Constants::timeFromTimeToSeconds(performanceTimer) + " seconds.");

    addMainTabForDataset(std::move(dataset));
}

void VolbxMain::actionImportDataTriggered()
{
    ImportData import(this);
    if (import.exec() == QDialog::Accepted)
        importDataset(import.getSelectedDataset());
}

QString VolbxMain::createNameForTab(const std::unique_ptr<Dataset>& dataset)
{
    QString nameForTabBar{dataset->getName()};
    if (auto [ok, column] = dataset->getTaggedColumn(ColumnTag::VALUE); ok)
        nameForTabBar.append(
            " (" + dataset->getHeaderName(static_cast<int>(column)) + ")");
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

    ui->statusBar->showMessage(datasetName + " " + tr("loaded"));
}

void VolbxMain::setupStatusBar()
{
    ui->statusBar->showMessage(tr("Ready") + "...");
}

bool VolbxMain::canUpdate(QNetworkReply* reply)
{
    if (!Networking::replyIsValid(reply))
    {
        ui->statusBar->showMessage(tr("Connection error encountered."));
        return false;
    }

    auto [newestVersion, notNeededHereList] =
        Networking::getAvailableVersionAndFiles(reply);
    if (newestVersion.isEmpty())
    {
        ui->statusBar->showMessage(tr("Wrong answer received from server."));
        return false;
    }

    if (QApplication::applicationVersion() == newestVersion)
    {
        ui->statusBar->showMessage(tr("No updates available"));
        return false;
    }

    return true;
}

void VolbxMain::updateApplication()
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
                Constants::getUpdaterName() + Constants::getExeFileSuffix() +
                ".\n" + tr("Can not use update functionality."));
    }
}

void VolbxMain::updateCheckReplyFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    if (!canUpdate(reply))
        return;

    QMessageBox::StandardButton answer{QMessageBox::question(
        nullptr, tr("New version"),
        tr("New version is available. Download and install it now?"))};

    if (QMessageBox::Yes == answer)
        updateApplication();
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
    auto* action{qobject_cast<QAction*>(sender())};
    QString style{action->text()};
    if (QStyleFactory::keys().contains(style))
        Application::setQtStyle(style);
    else
        Application::setCssStyle(style);
    Configuration::getInstance().setStyleName(style);
}
