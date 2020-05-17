#ifndef VOLBXMAIN_H
#define VOLBXMAIN_H

#include <memory>

#include <QDate>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QSet>

namespace Ui
{
class VolbxMain;
}  // namespace Ui

class FiltersDock;
class DataView;
class ViewDockWidget;
class TableModel;
class Dataset;
class QNetworkReply;
class FilteringProxyModel;
class MainTabWidget;

/**
 * @brief Volbx main window.
 */
class VolbxMain : public QMainWindow
{
    Q_OBJECT
public:
    explicit VolbxMain(QWidget* parent = nullptr);

    ~VolbxMain() override;

    VolbxMain& operator=(const VolbxMain& other) = delete;
    VolbxMain(const VolbxMain& other) = delete;

    VolbxMain& operator=(VolbxMain&& other) = delete;
    VolbxMain(VolbxMain&& other) = delete;

    void checkForUpdates();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    /**
     * @brief add new tab for given dataset.
     * @param dataset dataset.
     */
    void addMainTabForDataset(std::unique_ptr<Dataset> dataset);

    /// Side dock with filters.
    FiltersDock* filters_{nullptr};

    void manageActions(bool tabExists);

    Ui::VolbxMain* ui;

    void setupStatusBar();

    /// Main tab widget/center widget.
    MainTabWidget* tabWidget_{nullptr};

    /// Network manager used to retrieve current version available.
    QNetworkAccessManager networkManager_;

    void createOptionsMenu();

    void setStandardIcons();

private Q_SLOTS:
    void actionExitTriggered();

    void actionFiltersTriggered();

    void actionLogsTriggered();

    void tabWasChanged(int index);

    void actionAboutTriggered();

    void closeTab(int tab);

    void actionExportTriggered();

    void actionSaveDatasetAsTriggered();

    void actionImportDataTriggered();

    void updateCheckReplyFinished(QNetworkReply* reply);

    void actionCheckForNewVersionTriggered();

    void actionUpdateAutoToggled(bool alwaysCheck);

    /**
     * @brief picked qt built in style.
     */
    void qtStylePicked();

    /**
     * @brief picked custom style (found in resoures or application dir).
     */
    void customStylePicked();
};

#endif  // VOLBXMAIN_H
