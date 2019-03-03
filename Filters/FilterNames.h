#ifndef FILTERNAMES_H
#define FILTERNAMES_H

#include <QSet>

#include "Filter.h"

class QListWidgetItem;

namespace Ui {
    class FilterNames;
}

/**
 * @brief Names filter for string type columns in model.
 */
class FilterNames : public Filter
{
    Q_OBJECT
public:
    FilterNames(QString name,
                int column,
                const QStringList& initialList,
                QWidget *parent = nullptr);

    virtual ~FilterNames();

    virtual QSize sizeHint() const;

public slots:
    virtual void setChecked(bool checked);

private:
    Q_DISABLE_COPY(FilterNames)

    QStringList initialList_;

    QSet<QString> lastEmitted_;

    Ui::FilterNames *ui;

    ///Flag indocating if additionall place for scroll should be added.
    bool addMarginForScrollBar_;

    ///Min number of chars to include scrioll margin.
    static const int minNameWidthForScrollMargin_;

    ///Maximum widget heigh.
    static const int maximumHeigh_;

private slots:
    /**
     * Triggered when user click on item.
     * @param item item clicked.
     */
    void itemChecked(QListWidgetItem* item);

    void on_selectAll_toggled(bool checked);

signals:
    void newStringFilter(int kolumn, QSet<QString> bannedList);
};

#endif // FILTERNAMES_H
