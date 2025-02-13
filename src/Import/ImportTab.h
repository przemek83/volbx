#pragma once

#include <memory>

#include <QWidget>

class Dataset;
class ColumnsPreview;
class DatasetVisualization;

class ImportTab : public QWidget
{
    Q_OBJECT
public:
    std::unique_ptr<Dataset> getDataset();

protected:
    std::pair<DatasetVisualization*, ColumnsPreview*>
    createVisualizationAndPreview();

    void setDataset(std::unique_ptr<Dataset> dataset);

signals:
    void datasetIsReady(bool ready);
};
