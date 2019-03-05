#ifndef FILTERNAMES_H
#define FILTERNAMES_H

#include <QSet>

#include "Filter.h"

class QListWidgetItem;

namespace Ui
{
class FilterNames;
} // namespace Ui

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
                QWidget* parent = nullptr);

    ~FilterNames() override;

    FilterNames& operator=(const FilterNames& other) = delete;
    FilterNames(const FilterNames& other) = delete;

    FilterNames& operator=(FilterNames&& other) = delete;
    FilterNames(FilterNames&& other) = delete;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setChecked(bool checked) override;

private:
    QStringList initialList_;

    QSet<QString> lastEmitted_;

    Ui::FilterNames* ui;

    ///Flag indocating if additionall place for scroll should be added.
    bool addMarginForScrollBar_;

    ///Min number of chars to include scrioll margin.
    static const int minNameWidthForScrollMargin_;

    ///Maximum widget heigh.
    static const int maximumHeigh_;

private Q_SLOTS:
    /**
     * Triggered when user click on item.
     * @param item item clicked.
     */
    void itemChecked(QListWidgetItem* item);

    void on_selectAll_toggled(bool checked);

Q_SIGNALS:
    void newStringFilter(int kolumn, QSet<QString> bannedList);
};

#endif // FILTERNAMES_H
