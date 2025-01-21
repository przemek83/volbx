#include "FilteringProxyModel.h"

#include <QDate>

#include "TableModel.h"

const TableModel* FilteringProxyModel::getParentModel() const
{
    return ::qobject_cast<const TableModel*>(sourceModel());
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

bool FilteringProxyModel::acceptRowWithStringRestrictions(
    int sourceRow, const QModelIndex& sourceParent) const
{
    const auto condition{
        [this, &row = sourceRow,
         &parent = sourceParent](const auto& restriction)
        {
            const auto& [column, bannedStrings] = restriction;
            const QModelIndex index{sourceModel()->index(row, column, parent)};
            return bannedStrings.contains(index.data().toString());
        }};

    return std::none_of(stringsRestrictions_.cbegin(),
                        stringsRestrictions_.cend(), condition);
}

bool FilteringProxyModel::acceptRowWithDateRestrictions(
    int sourceRow, const QModelIndex& sourceParent) const
{
    for (const auto& [column, dateRestriction] : datesRestrictions_)
    {
        const QModelIndex index{
            sourceModel()->index(sourceRow, column, sourceParent)};
        const auto& [min, max, emptyDates] = dateRestriction;
        const QVariant& dateVariant{index.data()};
        if (dateVariant.isNull())
            return !emptyDates;

        const QDate itemDate{dateVariant.toDate()};
        if ((itemDate < min) || (itemDate > max))
            return false;
    }
    return true;
}

bool FilteringProxyModel::acceptRowWithNumberRestrictions(
    int sourceRow, const QModelIndex& sourceParent) const
{
    const auto condition{
        [this, &row = sourceRow,
         &parent = sourceParent](const auto& restriction)
        {
            const auto& [column, numericRestriction] = restriction;
            const QModelIndex index{sourceModel()->index(row, column, parent)};
            auto [min, max] = numericRestriction;
            const double itemDouble{
                QString::number(index.data().toDouble(), 'f', 2).toDouble()};
            return (itemDouble >= min) && (itemDouble <= max);
        }};

    return std::all_of(numericRestrictions_.cbegin(),
                       numericRestrictions_.cend(), condition);
}

bool FilteringProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex& source_parent) const
{
    return acceptRowWithStringRestrictions(source_row, source_parent) &&
           acceptRowWithDateRestrictions(source_row, source_parent) &&
           acceptRowWithNumberRestrictions(source_row, source_parent);
}
