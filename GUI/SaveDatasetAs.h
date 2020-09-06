#pragma once

#include <QDialog>

namespace Ui
{
class SaveDatasetAs;
}  // namespace Ui

/**
 * @brief Window for saving datasets.
 */
class SaveDatasetAs : public QDialog
{
    Q_OBJECT
public:
    explicit SaveDatasetAs(QStringList alreadyUsedNames,
                           QWidget* parent = nullptr);

    ~SaveDatasetAs() override;

    QString getChosenDatasetName();

private:
    Ui::SaveDatasetAs* ui;

    QStringList alreadyUsedNames_;

private Q_SLOTS:
    /**
     * @brief action on string/name change.
     * @param actualText actual string.
     */
    void nameLineEditTextChanged(const QString& actualText);

    void saveClicked();

    void cancelClicked();
};
