#ifndef GROUPPLOTGUI_H
#define GROUPPLOTGUI_H

#include "PlotDockWidget.h"

#include <QMap>
#include <QScrollArea>
#include <QRegExpValidator>

#include "Common/Quantiles.h"
#include "Common/Formats.h"
#include "Charts/GroupPlot.h"
#include "Charts/QuantilesPlot.h"

namespace Ui {
    class GroupPlotGui;
}

class TableModel;

/**
 * @brief Gui for grouping plot.
 * Contains functionality to change grouping column, fill interwals and so on.
 */
class GroupPlotGui : public PlotDockWidget
{
    Q_OBJECT
public:
    GroupPlotGui(const TableModel* model, QWidget *parent = nullptr);

    virtual ~GroupPlotGui();

public slots:
    void setNewData(float minY,
                    float maxY,
                    QVector<QString> intervalsNames,
                    QVector<Quantiles> quantilesForIntervals,
                    Quantiles quantiles);

private:
    Q_DISABLE_COPY(GroupPlotGui)

    Ui::GroupPlotGui *ui;

    GroupPlot groupPlot_;

    QuantilesPlot quantilesPlot_;

    QHash<int, DataFormat> columnsNumberToFormatMap_;

    class ScrollArea : public QScrollArea
    {
    public:
        explicit ScrollArea(QWidget* parent = nullptr);
        virtual ~ScrollArea() = default;
        void forceResize();
    };

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

private slots:
    void on_comboBox_currentIndexChanged(int index);

signals:
    void newGroupingColumn(int column);
};

#endif // GROUPPLOTGUI_H
