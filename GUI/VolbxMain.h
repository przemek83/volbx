#pragma once

#include <memory>

#include <QMainWindow>
#include <QNetworkAccessManager>

class QActionGroup;
namespace Ui
{
class VolbxMain;
}  // namespace Ui

class FiltersDock;
class Dataset;
class TabWidget;

/**
 * @brief Volbx main window.
 */
class VolbxMain : public QMainWindow
{
    Q_OBJECT
public:
    explicit VolbxMain(QWidget* parent = nullptr);

    ~VolbxMain() override;

    void checkForUpdates();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void connectFilter();

    void connectPlots();

    void connectActions();

    void setupTabWidget();

    void setupFilters();

    void setupNetworkManager();

    void addStyleToMenu(const QString& name, QActionGroup* actionsGroup);

    /**
     * @brief add new tab for given dataset.
     * @param dataset dataset.
     */
    void addMainTabForDataset(std::unique_ptr<Dataset> dataset);

    void manageActions(bool tabExists);

    void setupStatusBar();

    void createOptionsMenu();

    void addUpdatesSectionToMenu();

    void addStylesSectionToMenu();

    void addStylesFoundInAppDir(QActionGroup* actionsGroup);

    void addStandardQtStyles(QActionGroup* actionsGroup);

    void setStandardIcons();

    bool doesUserWantsToCheckForUpdates();

    void setTooltipsForChartsActions(bool chartsActive);

    void saveDataset(const QString& datasetName);

    Ui::VolbxMain* ui;

    /// Side dock with filters.
    FiltersDock* filters_{nullptr};

    /// Main tab widget/center widget.
    TabWidget* tabWidget_{nullptr};

    /// Network manager used to retrieve current version available.
    QNetworkAccessManager networkManager_;

private Q_SLOTS:
    void tabWasChanged(int index);

    void actionAboutTriggered();

    void closeTab(int tab);

    void actionExportTriggered();

    void actionSaveDatasetAsTriggered();

    void actionImportDataTriggered();

    void updateCheckReplyFinished(QNetworkReply* reply);

    void actionCheckForNewVersionTriggered();

    void actionUpdateAutoToggled(bool alwaysCheck);

    void styleChanged();
};
