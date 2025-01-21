#pragma once

#include <functional>
#include <memory>

#include <QDialog>
#include <QTabWidget>

#include "DatasetImportTab.h"
#include "SpreadsheetsImportTab.h"

class Dataset;
class QDialogButtonBox;
class QTabWidget;

class ImportData : public QDialog
{
    Q_OBJECT
public:
    explicit ImportData(const QString& importFilePath);

    std::unique_ptr<Dataset> getSelectedDataset();

    QString getImportFilePath() const;

private:
    QDialogButtonBox* createButtonBox();

    void setupLayout();

    void setupTabWidget(const std::function<void(bool)>& enableOpenButton);

    QTabWidget tabWidget_;

    DatasetImportTab datasetsTab_;

    SpreadsheetsImportTab spreadsheetsTab_;
};
