#ifndef SAVEDATASETAS_H
#define SAVEDATASETAS_H

#include <QDialog>

namespace Ui {
    class SaveDatasetAs;
}

/**
 * @brief Window in which user can enter name of dataset to save.
 */
class SaveDatasetAs : public QDialog
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param alreadyUsedNames names of existing datasets.
     * @param parent pointer to parent.
     */
    SaveDatasetAs(QStringList alreadyUsedNames, QWidget *parent = nullptr);

    virtual ~SaveDatasetAs();

    /**
     * @brief acces to entered by user name of dataset.
     * @return name.
     */
    QString getChosenDatasetName();

private slots:
    /**
     * @brief action on string/name change.
     * @param actualText actual string.
     */
    void on_nameLineEdit_textChanged(const QString &actualText);

    void on_save_clicked();

    void on_cancel_clicked();

private:
    Q_DISABLE_COPY(SaveDatasetAs)

    Ui::SaveDatasetAs *ui;

    ///Existing datasets names.
    QStringList alreadyUsedNames_;
};

#endif // SAVEDATASETAS_H
