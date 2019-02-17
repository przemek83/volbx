#ifndef FILTERINGPROXYMODEL_H
#define FILTERINGPROXYMODEL_H

#include <map>

#include <QSortFilterProxyModel>
#include <QStringList>
#include <QDate>

class TableModel;

/**
 * @brief Filtering model for 2d data.
 */
class FilteringProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FilteringProxyModel(QObject *parent = 0);

    virtual ~FilteringProxyModel() = default;

    /**
     * @brief get pointer to parent model.
     * @return parent parent model.
     */
    const TableModel* getParentModel() const;

public slots:
    /**
     * @brief set filter for string column.
     * @param column column number to set filter.
     * @param bannedList list of strings to filter.
     */
    void setStringFilter(int column, QSet<QString>& bannedStrings);

    /**
     * @brief set filter for date column.
     * @param column column number to set filter.
     * @param from filter from date.
     * @param to filter to date.
     */
    void setDateFilter(int column, QDate from, QDate to, bool filterEmptyDates);

    /**
     * @brief set filter for numeric column.
     * @param column column number to set filter.
     * @param from filter from value.
     * @param to filter to value.
     */
    void setNumericFilter(int column, double from, double to);

protected:
    /**
     * @brief Determine if row should be shown or not.
     * @param sourceRow row to check.
     * @param sourceParent index to check.
     * @return filter row (true), show row (false).
     */
    virtual bool filterAcceptsRow(int sourceRow,
                                  const QModelIndex &sourceParent) const;

private:
    Q_DISABLE_COPY(FilteringProxyModel)

    ///Filter set for strings.
    std::map<int, QSet<QString> > stringsRestrictions_;

    ///Filter set for dates.
    std::map<int, std::tuple<QDate, QDate, bool> > datesRestrictions_;

    ///Filter set for numeric.
    std::map<int, std::pair<double, double> > numericRestrictions_;
};

#endif // FILTERINGPROXYMODEL_H
