#pragma once

#include <QTableView>

#include "PlotDataProvider.h"

class TableModel;
class FilteringProxyModel;

class DataView : public QTableView
{
    Q_OBJECT
public:
    explicit DataView(QWidget* parent = nullptr);

    void setModel(QAbstractItemModel* model) override;

    const PlotDataProvider& getPlotDataProvider() const;

    void recomputeAllData();

public Q_SLOTS:
    void groupingColumnChanged(int column);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    QVector<TransactionData> fillDataFromSelection(int groupByColumn) const;

    void initHorizontalHeader();

    void initVerticalHeader();

    const FilteringProxyModel* getProxyModel() const;

    const TableModel* getParentModel() const;

    static std::tuple<bool, int, int> getTaggedColumns(
        const TableModel* parentModel);

    void createTransactionData() const;

    void setDelegate(int columnIndex, const TableModel* parentModel);

    int groupByColumn_{0};

    PlotDataProvider plotDataProvider_;
};
