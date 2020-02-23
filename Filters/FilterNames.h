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
    FilterNames(const QString& name,
                int column,
                QStringList initialList,
                QWidget* parent = nullptr);

    ~FilterNames() override;

    FilterNames& operator=(const FilterNames& other) = delete;
    FilterNames(const FilterNames& other) = delete;

    FilterNames& operator=(FilterNames&& other) = delete;
    FilterNames(FilterNames&& other) = delete;

    QSize sizeHint() const override;

private:
    const QStringList initialList_;

    QSet<QString> lastEmitted_;

    Ui::FilterNames* ui;

    ///Min number of chars to include scrioll margin.
    static constexpr int minNameWidthForScrollMargin_ {25};

    ///Maximum widget heigh.
    static constexpr int maximumHeigh_ {180};

    ///Flag indocating if additionall place for scroll should be added.
    bool addMarginForScrollBar_;

private Q_SLOTS:
    void setChecked(bool checked);

    /**
     * Triggered when user click on item.
     * @param item item clicked.
     */
    void itemChecked(QListWidgetItem* item);

    void selectAllToggled(bool checked);

Q_SIGNALS:
    void newStringFilter(int column, QSet<QString> bannedList);
};

#endif // FILTERNAMES_H
