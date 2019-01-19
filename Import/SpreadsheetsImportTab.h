#ifndef SPREADSHEETSIMPORTTAB_H
#define SPREADSHEETSIMPORTTAB_H

#include "ImportTab.h"

namespace Ui {
class SpreadsheetsImportTab;
}

/**
 * @brief Spreadsheets import tab.
 */
class SpreadsheetsImportTab : public ImportTab
{
    Q_OBJECT
public:
    explicit SpreadsheetsImportTab(QWidget *parent = 0);

    virtual ~SpreadsheetsImportTab();

    virtual DatasetDefinition* getDatasetDefinition();

private slots:
    void on_openFileButton_clicked();

private:
    Q_DISABLE_COPY(SpreadsheetsImportTab)

    Ui::SpreadsheetsImportTab *ui;
};

#endif // SPREADSHEETSIMPORTTAB_H
