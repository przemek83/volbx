#pragma once

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

    /**
     * @brief access to entered by user name of dataset.
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
