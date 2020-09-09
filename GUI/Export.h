#pragma once

#include <QDialog>

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
    explicit Export(QWidget* tab, QWidget* parent = nullptr);

    ~Export() override;

private:
    void saveOnDisk();

    bool locationIsValid(const QString& location) const;

    bool exportData(const QString& fileName);

    static bool exportToXlsx(const QString& fileName, const DataView* view,
                             ProgressBarCounter& bar);

    static bool exportToCsv(const QString& fileName, const DataView* view,
                            ProgressBarCounter& bar);

    void exportPlots(const QString& fileName);

    QString getFileName();

    Ui::Export* ui;

    QWidget* tab_;

    const QString exportFilesDateFormat_{QStringLiteral("yyyyMMdd")};

private Q_SLOTS:
    void saveClicked();

    void locationSearchClicked();
};
