#ifndef SPREADSHEETSIMPORTTAB_H
#define SPREADSHEETSIMPORTTAB_H

#include "ImportTab.h"

namespace Ui
{
class SpreadsheetsImportTab;
} // namespace Ui

/**
 * @brief Spreadsheets import tab.
 */
class SpreadsheetsImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit SpreadsheetsImportTab(QWidget* parent = nullptr);

    ~SpreadsheetsImportTab() override;

    SpreadsheetsImportTab& operator=(const SpreadsheetsImportTab& other) = delete;
    SpreadsheetsImportTab(const SpreadsheetsImportTab& other) = delete;

    SpreadsheetsImportTab& operator=(SpreadsheetsImportTab&& other) = delete;
    SpreadsheetsImportTab(SpreadsheetsImportTab&& other) = delete;

    DatasetDefinition* getDatasetDefinition() override;

private slots:
    void on_openFileButton_clicked();

private:
    Ui::SpreadsheetsImportTab* ui;
};

#endif // SPREADSHEETSIMPORTTAB_H
