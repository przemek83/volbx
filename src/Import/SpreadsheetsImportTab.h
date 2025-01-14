#pragma once

#include <memory>

#include "ImportTab.h"

#include "ui_SpreadsheetsImportTab.h"

class DatasetSpreadsheet;
class QFileInfo;

class SpreadsheetsImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit SpreadsheetsImportTab(QWidget* parent);

private:
    static void analyzeFile(std::unique_ptr<Dataset>& dataset);

    static std::unique_ptr<Dataset> createDataset(const QFileInfo& fileInfo);

    static bool fileIsOk(const QFileInfo& fileInfo);

    static QString getValidDatasetName(const QFileInfo& fileInfo);

    bool getFileInfo(QFileInfo& fileInfo);

    std::unique_ptr<Ui::SpreadsheetsImportTab> ui_;

private Q_SLOTS:
    void openFileButtonClicked();
};
