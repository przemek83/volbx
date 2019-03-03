#ifndef FILTERDATES_H
#define FILTERDATES_H

#include <QWidget>
#include <QDate>
#include <QCalendarWidget>

#include "Filter.h"

namespace Ui {
    class FilterDates;
}

/**
 * @brief filter for date type columns in model.
 */
class FilterDates : public Filter
{
    Q_OBJECT

public:
    FilterDates(QString name,
                int column,
                QDate min, QDate max,
                bool emptyDates,
                QWidget *parent = nullptr);

    virtual ~FilterDates();

public slots:
    virtual void setChecked(bool checked);

private:
    Q_DISABLE_COPY(FilterDates)

    ///Initial filter minimum date.
    QDate minOnInit_;

    ///Initial filter maximum date.
    QDate maxOnInit_;

    Ui::FilterDates *ui;

    QCalendarWidget calendarLeft_;

    QCalendarWidget calendarRight_;

    ///Flag indicating there were wmpty dates in column.
    bool emptyDates_;

private slots:

    /**
     * Trigerred on change of date in left date edit.
     * @param newDate new value.
     */
    void lowerDateChanged(QDate newDate);

    /**
     * Trigerred on change of date in right date edit.
     * @param newDate new value.
     */
    void higherDateChanged(QDate newDate);

    /**
     * Trigerred when empty dates checkbox state change.
     * @param checked flag.
     */
    void on_emptyDates_toggled(bool checked);

signals:
    void newDateFilter(int kolumn, QDate from, QDate to, bool filterEmptyDates);
};

#endif // FILTERDATES_H
