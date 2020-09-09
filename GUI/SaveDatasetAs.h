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
    explicit SaveDatasetAs(QStringList usedNames, QWidget* parent = nullptr);

    ~SaveDatasetAs() override;

    QString getDatasetName();

private:
    bool overwriteDataset(const QString& name);

    bool nameIsUsed(const QString& name);

    static void adjustWidgetBackgroundColor(QWidget* widget, bool nameUsed);

    Ui::SaveDatasetAs* ui;

    QStringList usedNames_;

private Q_SLOTS:
    /**
     * @brief action on string/name change.
     * @param actualText actual string.
     */
    void nameChanged(const QString& actualText);

    void saveClicked();

    void cancelClicked();
};
