#pragma once

#include <QSortFilterProxyModel>

class TableModel;

class FilteringProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FilteringProxyModel(QObject* parent = nullptr);

    const TableModel* getParentModel() const;

    void setStringFilter(int column, const QStringList& bannedStrings);

    void setDateFilter(int column, QDate from, QDate to, bool filterEmptyDates);

    void setNumericFilter(int column, double from, double to);

protected:
    bool filterAcceptsRow(int sourceRow,
                          const QModelIndex& sourceParent) const override;

private:
    bool acceptRowAccordingToStringRestrictions(
        int sourceRow, const QModelIndex& sourceParent) const;

    bool acceptRowAccordingToDateRestrictions(
        int sourceRow, const QModelIndex& sourceParent) const;

    bool acceptRowAccordingToNumericRestrictions(
        int sourceRow, const QModelIndex& sourceParent) const;

    std::map<int, QStringList> stringsRestrictions_;

    std::map<int, std::tuple<QDate, QDate, bool> > datesRestrictions_;

    std::map<int, std::pair<double, double> > numericRestrictions_;
};
