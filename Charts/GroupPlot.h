#ifndef GROUPPLOT_H
#define GROUPPLOT_H

#include <QEvent>

#include <qwt_scale_div.h>
#include <qwt_plot_curve.h>

#include "Common/Quantiles.h"

#include "CustomMarker.h"
#include "PlotBase.h"
#include "Picker.h"

class QwtPlotCurve;

/**
 * @brief Plot on which user can group by text columns.
 */
class GroupPlot : public PlotBase
{
public:
    explicit GroupPlot(QWidget* parent = 0);

    virtual ~GroupPlot();

    void setNewData(QVector<Quantiles> &quantiles,
                    QVector<QString> &intervalStrings);

    virtual QSize minimumSizeHint() const;

protected:
    virtual bool event(QEvent* event);

private:
    Q_DISABLE_COPY(GroupPlot)

    void shortenIntervalsNamesIfNeeded(QVector<QString>& intervalsNames,
                                       const QVector<Quantiles>& quantilesForIntervals);

    ///Maximum number of chars in label.
    const static int maxCharsInLabel_;

    CustomMarker* marker_;

    //Quantiles.
    QVector<Quantiles> quantiles_;

    //Names used on axis.
    QVector<QString> shortIntervalNames_;

    ///Names used in tooltip.
    QVector<QString> longIntervalNames_;

    class GroupPicker : public Picker
    {
    public:
        explicit GroupPicker(QWidget* parent);

        virtual ~GroupPicker() = default;

    protected:
        virtual QwtText trackerTextF(const QPointF &pos) const;
    };

    GroupPicker* picker_;
};
#endif // GROUPPLOT_H
