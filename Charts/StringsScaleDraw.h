#ifndef STRINGSSCALEDRAW_H
#define STRINGSSCALEDRAW_H

#include <qwt_scale_draw.h>

/**
 * @brief Scale drawing used in grouping plot. Rotates labels.
 */
class StringsScaleDraw : public QwtScaleDraw
{
public:
    explicit StringsScaleDraw(QVector<QString>* intervals);

    virtual ~StringsScaleDraw() = default;

    virtual QwtText label(double v) const;

private:
    Q_DISABLE_COPY(StringsScaleDraw)

    QVector<QString>* intervals_;
};

#endif // STRINGSSCALEDRAW_H
