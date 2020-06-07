#include "FilteringProxyModel.h"

#include <Constants.h>
#include <Logger.h>

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
    datesRestrictions_[column] = std::make_tuple(from, to, filterEmptyDates);
    invalidate();
}

void FilteringProxyModel::setNumericFilter(int column, double from, double to)
{
    numericRestrictions_[column] = std::make_pair(from, to);
    invalidate();
}

bool FilteringProxyModel::filterAcceptsRow(
    int sourceRow, const QModelIndex& sourceParent) const
{
    // Filter strings.
    std::map<int, QStringList>::const_iterator iterStrings;
    for (iterStrings = stringsRestrictions_.begin();
         iterStrings != stringsRestrictions_.end(); ++iterStrings)
    {
        QModelIndex index =
            sourceModel()->index(sourceRow, iterStrings->first, sourceParent);
        if (iterStrings->second.contains(index.data().toString()))
        {
            return false;
        }
    }

    // Filter dates.
    std::map<int, std::tuple<QDate, QDate, bool> >::const_iterator iterDates;
    for (iterDates = datesRestrictions_.begin();
         iterDates != datesRestrictions_.end(); ++iterDates)
    {
        QModelIndex index =
            sourceModel()->index(sourceRow, iterDates->first, sourceParent);
        std::tuple<QDate, QDate, bool> datesRestrictions = iterDates->second;
        const QVariant& dateVariant = index.data();

        if (dateVariant.isNull())
        {
            return !std::get<2>(datesRestrictions);
        }

        QDate itemDate = dateVariant.toDate();
        if (itemDate < std::get<0>(datesRestrictions) ||
            itemDate > std::get<1>(datesRestrictions))
        {
            return false;
        }
    }

    // Filter numbers.
    std::map<int, std::pair<double, double> >::const_iterator iterNumbers;
    for (iterNumbers = numericRestrictions_.begin();
         iterNumbers != numericRestrictions_.end(); ++iterNumbers)
    {
        QModelIndex index =
            sourceModel()->index(sourceRow, iterNumbers->first, sourceParent);
        std::pair<double, double> intPair = iterNumbers->second;
        double itemDouble =
            QString::number(index.data().toDouble(), 'f', 2).toDouble();
        if (itemDouble < intPair.first || itemDouble > intPair.second)
        {
            return false;
        }
    }

    return true;
}
