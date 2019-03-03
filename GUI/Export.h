#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>

class QMainWindow;

namespace Ui {
    class Export;
}

/**
 * @brief Export window. Exports data of active tab and all plots (if opened)
 */
class Export : public QDialog
{
    Q_OBJECT
public:
    Export(QMainWindow* tab, QWidget *parent = nullptr);

    ~Export() override;

private slots:
    void on_save_clicked();

    void on_locationSearch_clicked();

private:
    Q_DISABLE_COPY(Export)

    Ui::Export *ui;

    QMainWindow* tab_;

    void saveOnDisk();

    static const char* exportFilesDateFormat_;
};

#endif // EXPORT_H
