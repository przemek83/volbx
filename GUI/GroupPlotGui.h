#ifndef GROUPPLOTGUI_H
#define GROUPPLOTGUI_H

#include "PlotDockWidget.h"

#include <QMap>
#include <QRegExpValidator>

#include "Common/Quantiles.h"
#include "Common/Formats.h"
#include "Charts/GroupPlot.h"
#include "Charts/QuantilesPlot.h"

#include "ScrollArea.h"

namespace Ui
{
class GroupPlotGui;
} // namespace Ui

class TableModel;

/**
 * @brief Gui for grouping plot.
 * Contains functionality to change grouping column, fill interwals and so on.
 */
class GroupPlotGui : public PlotDockWidget
{
    Q_OBJECT
public:
    explicit GroupPlotGui(const TableModel* model, QWidget* parent = nullptr);

    ~GroupPlotGui() override;

    GroupPlotGui& operator=(const GroupPlotGui& other) = delete;
    GroupPlotGui(const GroupPlotGui& other) = delete;

    GroupPlotGui& operator=(GroupPlotGui&& other) = delete;
    GroupPlotGui(GroupPlotGui&& other) = delete;

public Q_SLOTS:
    void setNewData(float minY,
                    float maxY,
                    QVector<QString> intervalsNames,
                    const QVector<Quantiles>& quantilesForIntervals,
                    const Quantiles& quantiles);

private:
    Ui::GroupPlotGui* ui;

    GroupPlot groupPlot_;

    QuantilesPlot quantilesPlot_;

    QHash<int, DataFormat> columnsNumberToFormatMap_;

    ScrollArea* scrollArea_;

    bool getIntervals(QVector<float>* intervals);

    bool retrieveIntervals(QVector<float>* intervals);

    QRegExpValidator numValidator_;

    QRegExpValidator dateValidator_;

    static const char* numRegExp_;

    static const char* dateRegExp_;

    static const char* numPlaceHolder_;

    static const char* datePlaceHolder_;

    DataFormat getSelectedColumnFormat();

private Q_SLOTS:
    void on_comboBox_currentIndexChanged(int index);

Q_SIGNALS:
    void newGroupingColumn(int column);
};

#endif // GROUPPLOTGUI_H
