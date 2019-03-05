#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>

class QMainWindow;

namespace Ui
{
class Export;
} // namespace Ui

/**
 * @brief Export window. Exports data of active tab and all plots (if opened)
 */
class Export : public QDialog
{
    Q_OBJECT
public:
    explicit Export(QMainWindow* tab, QWidget* parent = nullptr);

    ~Export() override;

    Export& operator=(const Export& other) = delete;
    Export(const Export& other) = delete;

    Export& operator=(Export&& other) = delete;
    Export(Export&& other) = delete;

private Q_SLOTS:
    void on_save_clicked();

    void on_locationSearch_clicked();

private:
    Ui::Export* ui;

    QMainWindow* tab_;

    void saveOnDisk();

    static const char* exportFilesDateFormat_;
};

#endif // EXPORT_H
