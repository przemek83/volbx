#pragma once

#include <memory>

#include <QDialog>

#include "ui_SaveDatasetAs.h"

/**
 * @brief Window for saving datasets.
 */
class SaveDatasetAs : public QDialog
{
    Q_OBJECT
public:
    explicit SaveDatasetAs(QStringList usedNames, QWidget* parent = nullptr);

    QString getDatasetName();

private:
    bool overwriteDataset(const QString& name);

    bool nameIsUsed(const QString& name);

    static void adjustWidgetBackgroundColor(QWidget* widget, bool nameUsed);

    std::unique_ptr<Ui::SaveDatasetAs> ui_;

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
