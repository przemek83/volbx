#pragma once

#include <memory>

#include <QDialog>

#include "ui_SaveDatasetAs.h"

class SaveDatasetAs : public QDialog
{
    Q_OBJECT
public:
    explicit SaveDatasetAs(QStringList usedNames);

    QString getDatasetName() const;

private:
    bool overwriteDataset(const QString& name);

    bool nameIsUsed(const QString& name) const;

    static void adjustWidgetBackgroundColor(QWidget* widget, bool nameUsed);

    std::unique_ptr<Ui::SaveDatasetAs> ui_;

    QStringList usedNames_;

private slots:
    void nameChanged(const QString& actualText);

    void saveClicked();

    void cancelClicked();
};
