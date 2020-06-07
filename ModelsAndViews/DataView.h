#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QTableView>

#include "PlotDataProvider.h"

/**
 * @brief 2d view for data.
 */
class DataView : public QTableView
{
    Q_OBJECT
public:
    explicit DataView(QWidget* parent = nullptr);

    ~DataView() override = default;

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
     * @return vector of structures containing data, price and grouping data.
     */
    QVector<TransactionData> fillDataFromSelection(int groupByColumn) const;
};

#endif  // DATAVIEW_H
