#pragma once

#include <QTableView>

#include "PlotDataProvider.h"

class TableModel;
class FilteringProxyModel;

class DataView : public QTableView
{
    Q_OBJECT
public:
    explicit DataView(QWidget* parent);

    void setModel(QAbstractItemModel* model) override;

    const PlotDataProvider& getPlotDataProvider() const;

    void recomputeAllData();

public slots:
    void groupingColumnChanged(int column);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    QVector<TransactionData> fillDataFromSelection(int groupByColumn) const;

    void initHorizontalHeader() const;

    void initVerticalHeader() const;

    const FilteringProxyModel* getProxyModel() const;

    const TableModel* getParentModel() const;

    struct TaggedColumns
    {
        int date_{0};
        int value_{0};
    };

    static std::pair<bool, TaggedColumns> getTaggedColumns(
        const TableModel* parentModel);

    void setDelegate(int columnIndex, const TableModel* parentModel);

    int groupByColumn_{0};

    PlotDataProvider plotDataProvider_;
};
