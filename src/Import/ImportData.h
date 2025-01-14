#pragma once

#include <functional>
#include <memory>

#include <QDialog>

class Dataset;
class QDialogButtonBox;
class QTabWidget;

class ImportData : public QDialog
{
    Q_OBJECT
public:
    explicit ImportData(QWidget* parent = nullptr);

    std::unique_ptr<Dataset> getSelectedDataset();

    QString getZipFileName() const;

private:
    QDialogButtonBox* createButtonBox();

    void setupLayout();

    QTabWidget* createTabWidgetWithContent(
        const std::function<void(bool)>& enableOpenButton);
};
