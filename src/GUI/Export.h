#pragma once

#include <memory>

#include <QDialog>
#include <QRegularExpressionValidator>

#include "ui_Export.h"

class DataView;
class ProgressBarCounter;

/// @brief Exports data in active tab and all related plots.
class Export : public QDialog
{
    Q_OBJECT
public:
    Export(QWidget* tab, const QString& importFilePath, QWidget* parent);

private:
    void saveOnDisk() const;

    bool locationIsValid(const QString& location) const;

    bool exportData(const QString& fileName) const;

    static bool exportToXlsx(const QString& fileName, const DataView* view,
                             const ProgressBarCounter& bar);

    static bool exportToCsv(const QString& fileName, const DataView* view,
                            const ProgressBarCounter& bar);

    void exportPlots(const QString& fileName) const;

    QString getFileName() const;

    std::unique_ptr<Ui::Export> ui_{std::make_unique<Ui::Export>()};

    QWidget* tab_;

    const QString exportFilesDateFormat_{QStringLiteral("yyyyMMdd")};

    QRegularExpressionValidator validator_{
        QRegularExpression(QStringLiteral("[\\w]*"))};

private slots:
    void saveClicked();

    void locationSearchClicked();
};
