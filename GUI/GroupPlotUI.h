#ifndef GROUPPLOTUI_H
#define GROUPPLOTUI_H

#include <GroupPlot.h>
#include <QMap>
#include <QRegExpValidator>
#include <Quantiles.h>
#include <QuantilesPlot.h>
#include <QWidget>

namespace Ui
{
class GroupPlotUI;
}

class ScrollArea;

class GroupPlotUI : public QWidget
{
    Q_OBJECT
public:
    explicit GroupPlotUI(QVector<std::pair<QString, int>> stringColumns,
                         QWidget* parent = nullptr);
    ~GroupPlotUI() override;

    GroupPlotUI& operator=(const GroupPlotUI& other) = delete;
    GroupPlotUI(const GroupPlotUI& other) = delete;

    GroupPlotUI& operator=(GroupPlotUI&& other) = delete;
    GroupPlotUI(GroupPlotUI&& other) = delete;

public Q_SLOTS:
    void setNewData(float minY,
                    float maxY,
                    QVector<QString> intervalsNames,
                    const QVector<Quantiles>& quantilesForIntervals,
                    const Quantiles& quantiles);

private:
    Ui::GroupPlotUI* ui;

    GroupPlot groupPlot_;

    QuantilesPlot quantilesPlot_;

    ScrollArea* scrollArea_;

private Q_SLOTS:
    void comboBoxCurrentIndexChanged(int index);

Q_SIGNALS:
    void newGroupingColumn(int column);
};

#endif // GROUPPLOTUI_H
