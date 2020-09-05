#pragma once

#include <QDialog>

class QMainWindow;
class DataView;
class ProgressBarCounter;

namespace Ui
{
class Export;
}  // namespace Ui

/**
 * @brief Exports data in active tab and all related plots.
 */
class Export : public QDialog
{
    Q_OBJECT
public:
    explicit Export(QMainWindow* tab, QWidget* parent = nullptr);

    ~Export() override;

private:
    void saveOnDisk();

    bool locationIsValid(const QString& location) const;

    bool exportData(const QString& fileName);

    bool exportToXlsx(const QString& fileName, DataView* view,
                      ProgressBarCounter& bar);

    bool exportToCsv(const QString& fileName, DataView* view,
                     ProgressBarCounter& bar);

    void exportPlots(const QString& fileName);

    QString getFileName();

    Ui::Export* ui;

    QMainWindow* tab_;

    const QString exportFilesDateFormat_{QStringLiteral("yyyyMMdd")};

private Q_SLOTS:
    void saveClicked();

    void locationSearchClicked();
};
