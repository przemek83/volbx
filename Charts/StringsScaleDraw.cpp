#include "StringsScaleDraw.h"

#include <cmath>

#include "Constants.h"

StringsScaleDraw::StringsScaleDraw(QVector<QString>* intervals) :
    intervals_(intervals)
{
    setLabelRotation(Constants::DEFAULT_LABEL_ROTATION);
    setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
}

QwtText StringsScaleDraw::label(double v) const
{
    if (intervals_->isEmpty() || !Constants::doublesAreEqual(fmod(v, 1), 0.) ||
        Constants::doublesAreEqual(v, 0.) || v < 0 || v > intervals_->count())
    {
        return QwtText(QStringLiteral("                     "));
    }

    int point = static_cast<int>(v);
    if (intervals_->count() >= point)
    {
        return QwtText(intervals_->at(point - 1));
    }

    return QwtText(QStringLiteral("                     "));
}
