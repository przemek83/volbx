#pragma once

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

    /**
     * @brief Recompute data using currently selected rows.
     */
    void recomputeAllData();

public Q_SLOTS:
    /**
     * @brief Force recomputing of data because of grouping column changed.
     * @param column New grouping column.
     */
    void groupingColumnChanged(int column);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    /**
     * @brief Get data selected on view.
     * @param groupByColumn Column used in grouping.
     * @return Container of structures containing data, price and grouping data.
     */
    QVector<TransactionData> fillDataFromSelection(int groupByColumn) const;

    void initHorizontalHeader();

    void initVerticalHeader();

    const FilteringProxyModel* getProxyModel() const;

    const TableModel* getParentModel() const;

    std::tuple<bool, int, int> getTaggedColumns(
        const TableModel* parentModel) const;

    void createTransactionData() const;

    void setDelegate(int columnIndex, const TableModel* parentModel);

    int groupByColumn_{0};

    PlotDataProvider plotDataProvider_;
};
