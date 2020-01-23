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

public Q_SLOTS:
    void setChecked(bool checked);

private:
    const QStringList initialList_;

    QSet<QString> lastEmitted_;

    Ui::FilterNames* ui;

    ///Min number of chars to include scrioll margin.
    static const int minNameWidthForScrollMargin_;

    ///Maximum widget heigh.
    static const int maximumHeigh_;

    ///Flag indocating if additionall place for scroll should be added.
    bool addMarginForScrollBar_;

private Q_SLOTS:
    /**
     * Triggered when user click on item.
     * @param item item clicked.
     */
    void itemChecked(QListWidgetItem* item);

    void selectAllToggled(bool checked);

Q_SIGNALS:
    void newStringFilter(int kolumn, QSet<QString> bannedList);
};

#endif // FILTERNAMES_H
