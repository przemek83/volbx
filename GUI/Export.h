#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>

class QMainWindow;

namespace Ui
{
class Export;
}  // namespace Ui

/**
 * @brief Export window. Exports data of active tab and all plots (if opened)
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
    Ui::Export* ui;

    QMainWindow* tab_;

    void saveOnDisk();

    const QString exportFilesDateFormat_{QStringLiteral("yyyyMMdd")};
};

#endif  // EXPORT_H
