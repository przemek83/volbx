#pragma once

#include <memory>

#include <QWidget>

class Dataset;
class ColumnsPreview;
class DatasetVisualization;

/**
 * @brief Import tabs base class.
 */
class ImportTab : public QWidget
{
    Q_OBJECT
public:
    explicit ImportTab(QWidget* parent = nullptr);

    ~ImportTab() override = default;

    std::unique_ptr<Dataset> getDataset();

protected:
    std::pair<DatasetVisualization*, ColumnsPreview*>
    createVisualizationAndColumnPreview();

Q_SIGNALS:
    void datasetIsReady(bool);
};
