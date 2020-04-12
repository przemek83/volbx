#ifndef SAVEDATASETAS_H
#define SAVEDATASETAS_H

#include <QDialog>

namespace Ui
{
class SaveDatasetAs;
}  // namespace Ui

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
    explicit SaveDatasetAs(QStringList alreadyUsedNames,
                           QWidget* parent = nullptr);

    ~SaveDatasetAs() override;

    SaveDatasetAs& operator=(const SaveDatasetAs& other) = delete;
    SaveDatasetAs(const SaveDatasetAs& other) = delete;

    SaveDatasetAs& operator=(SaveDatasetAs&& other) = delete;
    SaveDatasetAs(SaveDatasetAs&& other) = delete;

    /**
     * @brief acces to entered by user name of dataset.
     * @return name.
     */
    QString getChosenDatasetName();

private Q_SLOTS:
    /**
     * @brief action on string/name change.
     * @param actualText actual string.
     */
    void nameLineEditTextChanged(const QString& actualText);

    void saveClicked();

    void cancelClicked();

private:
    Ui::SaveDatasetAs* ui;

    /// Existing datasets names.
    QStringList alreadyUsedNames_;
};

#endif  // SAVEDATASETAS_H
