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

    ~StringsScaleDraw() override = default;

    StringsScaleDraw& operator=(const StringsScaleDraw& other) = delete;
    StringsScaleDraw(const StringsScaleDraw& other) = delete;

    StringsScaleDraw& operator=(StringsScaleDraw&& other) = delete;
    StringsScaleDraw(StringsScaleDraw&& other) = delete;

    QwtText label(double v) const override;

private:
    QVector<QString>* intervals_;
};

#endif // STRINGSSCALEDRAW_H
