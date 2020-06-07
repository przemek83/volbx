#include "FilteringProxyModel.h"

#include <QDate>

#include "TableModel.h"

FilteringProxyModel::FilteringProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

const TableModel* FilteringProxyModel::getParentModel() const
{
    return qobject_cast<const TableModel*>(sourceModel());
}

void FilteringProxyModel::setStringFilter(int column,
                                          const QStringList& bannedStrings)
{
    stringsRestrictions_[column] = bannedStrings;
    invalidate();
}

void FilteringProxyModel::setDateFilter(int column, QDate from, QDate to,
                                        bool filterEmptyDates)
{
    datesRestrictions_[column] = {from, to, filterEmptyDates};
    invalidate();
}

void FilteringProxyModel::setNumericFilter(int column, double from, double to)
{
    numericRestrictions_[column] = {from, to};
    invalidate();
}

bool FilteringProxyModel::filterAcceptsRow(
    int sourceRow, const QModelIndex& sourceParent) const
{
    // Filter strings.
    for (const auto& [column, bannedStrings] : stringsRestrictions_)
    {
        QModelIndex index =
            sourceModel()->index(sourceRow, column, sourceParent);
        if (bannedStrings.contains(index.data().toString()))
            return false;
    }

    // Filter dates.
    for (const auto& [column, dateRestriction] : datesRestrictions_)
    {
        QModelIndex index =
            sourceModel()->index(sourceRow, column, sourceParent);
        std::tuple<QDate, QDate, bool> datesRestrictions = dateRestriction;
        const QVariant& dateVariant = index.data();

        if (dateVariant.isNull())
            return !std::get<2>(datesRestrictions);

        QDate itemDate = dateVariant.toDate();
        if (itemDate < std::get<0>(datesRestrictions) ||
            itemDate > std::get<1>(datesRestrictions))
            return false;
    }

    // Filter numbers.
    for (const auto& [column, numericRestriction] : numericRestrictions_)
    {
        QModelIndex index =
            sourceModel()->index(sourceRow, column, sourceParent);
        std::pair<double, double> intPair = numericRestriction;
        double itemDouble =
            QString::number(index.data().toDouble(), 'f', 2).toDouble();
        if (itemDouble < intPair.first || itemDouble > intPair.second)
            return false;
    }

    return true;
}
