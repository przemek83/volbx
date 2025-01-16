#pragma once

#include <QSortFilterProxyModel>

class TableModel;

class FilteringProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    using QSortFilterProxyModel::QSortFilterProxyModel;

    const TableModel* getParentModel() const;

    void setStringFilter(int column, const QStringList& bannedStrings);

    void setDateFilter(int column, QDate from, QDate to, bool filterEmptyDates);

    void setNumericFilter(int column, double from, double to);

protected:
    bool filterAcceptsRow(int source_row,
                          const QModelIndex& source_parent) const override;

private:
    bool acceptRowWithStringRestrictions(int sourceRow,
                                         const QModelIndex& sourceParent) const;

    bool acceptRowWithDateRestrictions(int sourceRow,
                                       const QModelIndex& sourceParent) const;

    bool acceptRowWithNumberRestrictions(int sourceRow,
                                         const QModelIndex& sourceParent) const;

    std::map<int, QStringList> stringsRestrictions_;

    std::map<int, std::tuple<QDate, QDate, bool> > datesRestrictions_;

    std::map<int, std::pair<double, double> > numericRestrictions_;
};
