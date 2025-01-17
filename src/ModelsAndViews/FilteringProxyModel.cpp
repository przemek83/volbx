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
    for (const auto& [column, bannedStrings] : stringsRestrictions_)
    {
        const QModelIndex index{
            sourceModel()->index(sourceRow, column, sourceParent)};
        if (bannedStrings.contains(index.data().toString()))
            return false;
    }
    return true;
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
    for (const auto& [column, numericRestriction] : numericRestrictions_)
    {
        const QModelIndex index{
            sourceModel()->index(sourceRow, column, sourceParent)};
        auto [min, max] = numericRestriction;
        const double itemDouble{
            QString::number(index.data().toDouble(), 'f', 2).toDouble()};
        if ((itemDouble < min) || (itemDouble > max))
            return false;
    }
    return true;
}

bool FilteringProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex& source_parent) const
{
    return acceptRowWithStringRestrictions(source_row, source_parent) &&
           acceptRowWithDateRestrictions(source_row, source_parent) &&
           acceptRowWithNumberRestrictions(source_row, source_parent);
}
