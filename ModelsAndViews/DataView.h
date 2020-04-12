#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <memory>

#include <QTableView>
#include <QVariant>
#include <QVector>

#include "DataProvider/PlotDataProvider.h"
#include "DataProvider/TransactionData.h"

class PlotDataProvider;
class TrendPlot;
class QMainWindow;
class QDockWidget;
class TableModel;

/**
 * @brief 2d view for data.
 */
class DataView : public QTableView
{
    Q_OBJECT
public:
    explicit DataView(QWidget* parent = nullptr);

    ~DataView() override = default;

    DataView& operator=(const DataView& other) = delete;
    DataView(const DataView& other) = delete;

    DataView& operator=(DataView&& other) = delete;
    DataView(DataView&& other) = delete;

    void setModel(QAbstractItemModel* model) override;

    const PlotDataProvider* getPlotDataProvider();

    void reloadSelectionDataAndRecompute();

public Q_SLOTS:
    void groupingColumnChanged(int column);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    /// Object for recomputing plot data.
    PlotDataProvider plotDataProvider_;

    /**
     * @brief get selected on view data.
     * @param groupByColumn column used in grouping.
     * @return vector of structs containg data, price and grouping data.
     */
    QVector<TransactionData> fillDataFromSelection(int groupByColumn) const;
};

#endif  // DATAVIEW_H
