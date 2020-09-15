#pragma once

#include <functional>
#include <memory>

#include <QDialog>

class Dataset;
class QDialogButtonBox;
class QTabWidget;

/**
 * @brief Main dialog for loading data into application.
 * It consist QTabWidget with pages for each type of data.
 */
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
        std::function<void(bool)> enableOpenButton);
};
