#pragma once

#include "ImportTab.h"

namespace Ui
{
class SpreadsheetsImportTab;
}  // namespace Ui

class DatasetSpreadsheet;
class QFileInfo;

/**
 * @brief Ui class for importing spreadsheets.
 */
class SpreadsheetsImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit SpreadsheetsImportTab(QWidget* parent = nullptr);

    ~SpreadsheetsImportTab() override;

private:
    static void analyzeFile(std::unique_ptr<Dataset>& dataset);

    static std::unique_ptr<Dataset> createDataset(const QFileInfo& fileInfo);

    static bool fileIsOk(const QFileInfo& fileInfo);

    static QString getValidDatasetName(const QFileInfo& fileInfo);

    bool getFileInfo(QFileInfo& fileInfo);

    Ui::SpreadsheetsImportTab* ui;

private Q_SLOTS:
    void openFileButtonClicked();
};
