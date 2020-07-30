#include "VolbxMain.h"

#include <ProgressBarCounter.h>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QStyleFactory>
#include <QUrl>
#include <QVariant>

#include "Common/Configuration.h"
#include "Common/DatasetUtilities.h"
#include "Common/ExportUtilities.h"
#include "Datasets/Dataset.h"
#include "Datasets/DatasetInner.h"
#include "Datasets/DatasetSpreadsheet.h"
#include "FiltersDock.h"
#include "Import/ImportData.h"
#include "ModelsAndViews/DataView.h"
#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"
#include "Shared/Application.h"
#include "Shared/Debugging.h"
#include "Shared/Logger.h"
#include "Shared/Networking.h"

#include "About.h"
#include "CheckUpdatesDialog.h"
#include "Export.h"
#include "MainTab.h"
#include "MainTabWidget.h"
#include "SaveDatasetAs.h"
#include "ViewDockWidget.h"
#include "ui_VolbxMain.h"

VolbxMain::VolbxMain(QWidget* parent)
    : QMainWindow(parent),
      filters_(nullptr),
      ui(new Ui::VolbxMain),
      tabWidget_(nullptr)
{
    ui->setupUi(this);

    setStandardIcons();

    setupStatusBar();

    filters_ = new FiltersDock(this);

    tabWidget_ = new MainTabWidget(this);

    connect(filters_, &FiltersDock::newNamesFiltering, tabWidget_,
            &MainTabWidget::setTextFilterInProxy);
    connect(filters_, &FiltersDock::newDateFiltering, tabWidget_,
            &MainTabWidget::setDateFilterInProxy);
    connect(filters_, &FiltersDock::newNumbersFiltering, tabWidget_,
            &MainTabWidget::setNumericFilterInProxy);

    connect(ui->actionBasic_plot, &QAction::triggered, tabWidget_,
            &MainTabWidget::addBasicPlot);
    connect(ui->actionHistogram, &QAction::triggered, tabWidget_,
            &MainTabWidget::addHistogramPlot);
    connect(ui->actionGroup_plot, &QAction::triggered, tabWidget_,
            &MainTabWidget::addGroupingPlot);

    ui->verticalLayout->addWidget(tabWidget_);

    addDockWidget(Qt::LeftDockWidgetArea, filters_);
    const int defaultFilterWidth{200};
    filters_->titleBarWidget()->resize(defaultFilterWidth,
                                       filters_->titleBarWidget()->height());

    connect(tabWidget_, &MainTabWidget::currentChanged, this,
            &VolbxMain::tabWasChanged);
    connect(tabWidget_, &MainTabWidget::tabCloseRequested, this,
            &VolbxMain::closeTab);

#ifndef DEBUGGING
    ui->actionLogs->setVisible(false);
#endif

    ui->actionBugReport->setVisible(false);
    ui->actionSendErrorOrIdea->setVisible(false);
    ui->actionTutorials->setVisible(false);

    connect(&networkManager_, &QNetworkAccessManager::finished, this,
            &VolbxMain::updateCheckReplyFinished);

    connect(ui->actionExit, &QAction::triggered, this,
            &VolbxMain::actionExitTriggered);
    connect(ui->actionFilters, &QAction::triggered, this,
            &VolbxMain::actionFiltersTriggered);
    connect(ui->actionLogs, &QAction::triggered, this,
            &VolbxMain::actionLogsTriggered);
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

    createOptionsMenu();
}

VolbxMain::~VolbxMain()
{
    delete ui;
    delete filters_;
}

void VolbxMain::setStandardIcons()
{
    QStyle* style = QApplication::style();
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

void VolbxMain::createOptionsMenu()
{
    ui->menuOptions->addSection(tr("Updates"));
    ui->menuOptions->addAction(ui->actionCheckForNewVersion);
    ui->menuOptions->addAction(ui->actionUpdateAuto);

    ui->menuOptions->addSection(tr("Styles"));

    QString activeStyl = Configuration::getInstance().getStyle();

    auto actionsGroup = new QActionGroup(this);

    // Add orange style.
    QString styleName(QStringLiteral("Dark Orange"));
    auto action = new QAction(styleName, actionsGroup);
    action->setCheckable(true);
    if (activeStyl == styleName)
    {
        action->setChecked(true);
    }
    connect(action, &QAction::triggered, this, &VolbxMain::customStylePicked);

    // Add blue style.
    styleName = QStringLiteral("Rounded Blue");
    action = new QAction(styleName, actionsGroup);
    action->setCheckable(true);
    if (activeStyl == styleName)
    {
        action->setChecked(true);
    }
    connect(action, &QAction::triggered, this, &VolbxMain::customStylePicked);

    // Add styles found in app dir.
    QStringList nameFilter(QStringLiteral("*.css"));
    QDir directory(QCoreApplication::applicationDirPath());
    QFileInfoList styleFiles = directory.entryInfoList(nameFilter);

    for (const QFileInfo& styleFile : styleFiles)
    {
        action = new QAction(styleFile.baseName(), actionsGroup);
        action->setCheckable(true);
        if (activeStyl == styleFile.baseName())
        {
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this,
                &VolbxMain::customStylePicked);
    }

    // Add qt available styles.
    QStringList qtStylesList = QStyleFactory::keys();
    for (const QString& style : qtStylesList)
    {
        action = new QAction(style, actionsGroup);
        action->setCheckable(true);
        if (activeStyl == style)
        {
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this, &VolbxMain::qtStylePicked);
    }

    ui->menuOptions->addActions(actionsGroup->actions());
}

void VolbxMain::checkForUpdates()
{
    bool checkForUpdates = false;

    if (Configuration::getInstance().needToShowUpdatePickerDialog())
    {
        CheckUpdatesDialog dialog(this);

        int reply = dialog.exec();

        if (QDialog::Accepted == reply)
            checkForUpdates = true;

        // Remember if choice was checked.
        if (dialog.saveFlagSet())
            Configuration::getInstance().setUpdatesCheckingOption(
                checkForUpdates);
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

void VolbxMain::actionLogsTriggered()
{
#ifdef DEBUGGING
    Logger::getInstance()->switchVisibility();
#endif
}

void VolbxMain::tabWasChanged(int index)
{
    if (-1 != index)
    {
        const FilteringProxyModel* model = tabWidget_->getCurrentProxyModel();
        if (nullptr == model)
        {
            return;
        }
        filters_->activateFiltersForModel(model);
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
    QWidget* tabToDelete = tabWidget_->widget(tab);
    const FilteringProxyModel* model = tabWidget_->getCurrentProxyModel();
    filters_->removeModel(model);
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
                (activateCharts && dataModel->isAnySpecialColumnSet());
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
    if (!DatasetUtilities::isDatasetDirExistAndUserHavePermisions())
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
        QTime performanceTimer;
        performanceTimer.start();

        QString name{save.getChosenDatasetName()};
        LOG(LogTypes::IMPORT_EXPORT, "Saving dataset " + name);
        QString filePath{DatasetUtilities::getDatasetsDir() + name +
                         DatasetUtilities::getDatasetExtension()};
        ExportUtilities::saveDataset(filePath, view, &bar);

        LOG(LogTypes::IMPORT_EXPORT,
            "File saved in total time " +
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
        "Loaded file having " + QString::number(rowsCount_) + " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    addMainTabForDataset(std::move(dataset));
}

void VolbxMain::addMainTabForDataset(std::unique_ptr<Dataset> dataset)
{
    QString nameForTabBar{dataset->getName()};
    auto [exist, column] =
        dataset->getSpecialColumn(SpecialColumn::PRICE_PER_UNIT);
    if (exist)
        nameForTabBar.append(" (" + dataset->getHeaderName(column) + ")");

    QString datasetName{dataset->getName()};
    auto mainTab = new MainTab(std::move(dataset), tabWidget_);
    const FilteringProxyModel* proxyModel = mainTab->getCurrentProxyModel();
    if (nullptr != proxyModel)
    {
        filters_->addModel(proxyModel);
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
    if (Networking::errorsOccuredCheck(reply))
    {
        ui->statusBar->showMessage(tr("Connection error encountered."));
        return;
    }

    auto [newestVersion, notNeededHereList] =
        Networking::checkReplyAndReturnAvailableVersion(reply);

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
    Configuration::getInstance().setUpdatesCheckingOption(alwaysCheck);
}

void VolbxMain::qtStylePicked()
{
    auto action = qobject_cast<QAction*>(sender());
    if (nullptr != action)
    {
        QString style = action->text();
        Application::setQtStyle(style);
        Configuration::getInstance().setStyle(style);
    }
}

void VolbxMain::customStylePicked()
{
    auto action = qobject_cast<QAction*>(sender());
    if (nullptr != action)
    {
        QString styleName = action->text();
        Application::setCssStyle(styleName);
        Configuration::getInstance().setStyle(styleName);
    }
}
