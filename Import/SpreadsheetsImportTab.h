#pragma once

#include "ImportTab.h"

namespace Ui
{
class SpreadsheetsImportTab;
}  // namespace Ui

class DatasetSpreadsheet;
class QFileInfo;

class SpreadsheetsImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit SpreadsheetsImportTab(QWidget* parent = nullptr);

    ~SpreadsheetsImportTab() override;

private:
    void analyzeFile(std::unique_ptr<Dataset>& dataset);

    std::unique_ptr<Dataset> createDataset(const QFileInfo& fileInfo);

    bool fileIsOk(const QFileInfo& fileInfo);

    QString getValidDatasetName(const QFileInfo& fileInfo);

    bool getFileInfo(QFileInfo& fileInfo);

    Ui::SpreadsheetsImportTab* ui;

private Q_SLOTS:
    void openFileButtonClicked();
};
