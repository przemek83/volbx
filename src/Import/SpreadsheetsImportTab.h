#pragma once

#include <memory>

#include <QSplitter>

#include "ImportTab.h"

class DatasetSpreadsheet;
class QFileInfo;

namespace Ui
{
class SpreadsheetsImportTab;
}

class SpreadsheetsImportTab : public ImportTab
{
    Q_OBJECT
public:
    SpreadsheetsImportTab();

    ~SpreadsheetsImportTab() override;

private:
    static void analyzeFile(const std::unique_ptr<Dataset>& dataset);

    static std::unique_ptr<Dataset> createDataset(const QFileInfo& fileInfo);

    static bool fileIsOk(const QFileInfo& fileInfo);

    static QString getValidDatasetName(const QFileInfo& fileInfo);

    bool getFileInfo(QFileInfo& fileInfo);

    std::unique_ptr<Ui::SpreadsheetsImportTab> ui_{nullptr};

    QSplitter centralSplitter_{Qt::Vertical};

private slots:
    void openFileButtonClicked();
};
