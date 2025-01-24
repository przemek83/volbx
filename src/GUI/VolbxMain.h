#pragma once

#include <memory>

#include <QMainWindow>
#include <QNetworkAccessManager>

#include <FiltersDock.h>
#include <TabWidget.h>

#include "Configuration.h"

class QActionGroup;
class Dataset;

namespace Ui
{
class VolbxMain;
}  // namespace Ui

class VolbxMain : public QMainWindow
{
    Q_OBJECT
public:
    explicit VolbxMain(Configuration config);

    ~VolbxMain() override;

    void checkForUpdates();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void connectFilter() const;

    void connectPlots() const;

    void connectActions();

    void setupTabWidget();

    void setupFilters();

    void setupNetworkManager() const;

    void addStyleToMenu(const QString& name, QActionGroup* actionsGroup) const;

    void addMainTabForDataset(std::unique_ptr<Dataset> dataset);

    void manageActions(bool tabExists);

    void setupStatusBar();

    void createOptionsMenu();

    void addUpdatesSectionToMenu();

    void addStylesSectionToMenu();

    void addStylesFoundInAppDir(QActionGroup* actionsGroup) const;

    void addStandardQtStyles(QActionGroup* actionsGroup) const;

    void setStandardIcons();

    bool doesUserWantsToCheckForUpdates();

    void setTooltipsForChartsActions(bool chartsActive);

    void saveDataset(const QString& datasetName) const;

    void importDataset(std::unique_ptr<Dataset> dataset);

    bool canUpdate(QNetworkReply* reply);

    static void updateApplication();

    std::unique_ptr<Ui::VolbxMain> ui_;

    /// Side dock with filters.
    FiltersDock filters_;

    /// Main tab widget/centre widget.
    TabWidget tabWidget_;

    /// Network manager used to retrieve current available version.
    QNetworkAccessManager networkManager_;

    Configuration config_;

private slots:
    void tabWasChanged(int index);

    void actionAboutTriggered();

    void closeTab(int tab);

    void actionExportTriggered();

    void actionSaveDatasetAsTriggered();

    void actionImportDataTriggered();

    void updateCheckReplyFinished(QNetworkReply* reply);

    void actionCheckForUpdateTriggered();

    void actionUpdateAutoToggled(bool alwaysCheck);

    void styleChanged();
};
