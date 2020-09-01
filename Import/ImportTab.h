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

    virtual std::unique_ptr<Dataset> getDataset() = 0;

protected:
    std::pair<DatasetVisualization*, ColumnsPreview*>
    createVisualizationAndColumnPreview();

Q_SIGNALS:
    void definitionIsReady(bool);
};
