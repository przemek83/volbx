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
    explicit DataView(QWidget *parent = 0);

    virtual ~DataView();

    virtual void setModel(QAbstractItemModel* model);

    const PlotDataProvider* getPlotDataProvider();

    void reloadSelectionDataAndRecompute();

public slots:
    void groupingColumnChanged(int column);

protected:
    virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void keyPressEvent(QKeyEvent* event);

private:
    Q_DISABLE_COPY(DataView)

    ///Object for recomputing plot data.
    PlotDataProvider* plotDataProvider_;

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
