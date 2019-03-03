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

    QuantilesPlot& operator=(const QuantilesPlot& other) = delete;
    QuantilesPlot(const QuantilesPlot& other) = delete;

    QuantilesPlot& operator=(QuantilesPlot&& other) = delete;
    QuantilesPlot(QuantilesPlot&& other) = delete;

    void setNewData(Quantiles quantiles);

    QSize minimumSizeHint() const override;

    void forceResize();

private:
    class IntervalsScaleDraw: public QwtScaleDraw
    {
    public:
        IntervalsScaleDraw(int count);

        ~IntervalsScaleDraw() override = default;

        IntervalsScaleDraw& operator=(const IntervalsScaleDraw& other) = delete;
        IntervalsScaleDraw(const IntervalsScaleDraw& other) = delete;

        IntervalsScaleDraw& operator=(IntervalsScaleDraw&& other) = delete;
        IntervalsScaleDraw(IntervalsScaleDraw&& other) = delete;

        QwtText label(double v) const override;

    private:
        int count_;
    };

    CustomMarker* marker_ {nullptr};

    //One, but marker expects vector.
    QVector<Quantiles> quantiles_;

    class BoxPicker : Picker
    {
    public:
        explicit BoxPicker(QWidget* parent);

        ~BoxPicker() override = default;

        BoxPicker& operator=(const BoxPicker& other) = delete;
        BoxPicker(const BoxPicker& other) = delete;

        BoxPicker& operator=(BoxPicker&& other) = delete;
        BoxPicker(BoxPicker&& other) = delete;

    protected:
        QwtText trackerTextF(const QPointF& pos) const override;
    };

    BoxPicker* picker_;
};

#endif // QUANTILESPLOT_H
