#pragma once

#include <QDialog>

class QMainWindow;

namespace Ui
{
class Export;
}  // namespace Ui

/**
 * @brief Exports data of active tab and all plots.
 */
class Export : public QDialog
{
    Q_OBJECT
public:
    explicit Export(QMainWindow* tab, QWidget* parent = nullptr);

    ~Export() override;

private Q_SLOTS:
    void saveClicked();

    void locationSearchClicked();

private:
    void saveOnDisk();

    Ui::Export* ui;

    QMainWindow* tab_;

    const QString exportFilesDateFormat_{QStringLiteral("yyyyMMdd")};
};
