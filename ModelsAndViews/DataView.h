#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QTableView>
#include <QVector>
#include <QVariant>

#include "DataProvider/TransactionData.h"

class PlotDataProvider;
class TrendPlot;
class QMainWindow;
class QDockWidget;
class PlotDockWidget;
class TableModel;

/**
 * @brief 2d view for data.
 */
class DataView : public QTableView
{
    Q_OBJECT
public:
    explicit DataView(QWidget* parent = nullptr);

    ~DataView() override;

    DataView& operator=(const DataView& other) = delete;
    DataView(const DataView& other) = delete;

    DataView& operator=(DataView&& other) = delete;
    DataView(DataView&& other) = delete;

    void setModel(QAbstractItemModel* model) override;

    const PlotDataProvider* getPlotDataProvider();

    void reloadSelectionDataAndRecompute();

public slots:
    void groupingColumnChanged(int column);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    ///Object for recomputing plot data.
    PlotDataProvider* plotDataProvider_ {nullptr};

    /**
     * @brief get selected on view data.
     * @param groupByColumn column used in grouping.
     * @return vector of structs containg data, price and grouping data.
     */
    QVector<TransactionData>* fillDataFromSelection(int groupByColumn);

private slots:
    void showSortIndicatorIfNeeded(int section);
};

#endif // DATAVIEW_H
