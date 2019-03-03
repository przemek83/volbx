#ifndef QUANTILESPLOT_H
#define QUANTILESPLOT_H

#include <qwt_scale_draw.h>

#include "Common/Quantiles.h"

#include "PlotBase.h"
#include "Picker.h"

class CustomMarker;

/**
 * @brief plot showing quantiles (q25, q50, q75, min, max, avg).
 */
class QuantilesPlot : public PlotBase
{
public:
    explicit QuantilesPlot(QWidget* parent = nullptr);

    ~QuantilesPlot() override;

    void setNewData(Quantiles quantiles);

    QSize minimumSizeHint() const override;

    void forceResize();

private:
    Q_DISABLE_COPY(QuantilesPlot)

    class IntervalsScaleDraw: public QwtScaleDraw
    {
    public:
        IntervalsScaleDraw(int count);

        ~IntervalsScaleDraw() override = default;

        QwtText label(double v) const override;

    private:
        int count_;
    };

    CustomMarker* marker_;

    //One, but marker expects vector.
    QVector<Quantiles> quantiles_;

    class BoxPicker : Picker
    {
    public:
        BoxPicker(QWidget* parent);
        ~BoxPicker() override = default;

    protected:
        QwtText trackerTextF(const QPointF& pos) const override;
    };

    BoxPicker* picker_;
};

#endif // QUANTILESPLOT_H
