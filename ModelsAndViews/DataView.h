#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <QTableView>

#include "PlotDataProvider.h"

class TableModel;
class FilteringProxyModel;

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

    const PlotDataProvider& getPlotDataProvider() const;

    void reloadSelectionDataAndRecompute();

public Q_SLOTS:
    void groupingColumnChanged(int column);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    /**
     * @brief get selected on view data.
     * @param groupByColumn column used in grouping.
     * @return vector of structures containing data, price and grouping data.
     */
    QVector<TransactionData> fillDataFromSelection(int groupByColumn) const;

    void initHorizontalHeader();

    void initVerticalHeader();

    const FilteringProxyModel* getProxyModel() const;

    const TableModel* getParentModel() const;

    std::tuple<bool, int, int> getSpecialColumns(
        const TableModel* parentModel) const;

    void createTransactionData() const;

    void setDelegate(int columnIndex, const TableModel* parentModel);

    int groupByColumn_{0};

    PlotDataProvider plotDataProvider_;
};

#endif  // DATAVIEW_H
