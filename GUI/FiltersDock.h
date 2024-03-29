#pragma once

#include <QDate>
#include <QMap>
#include <QStackedWidget>

#include "GUI/Dock.h"

class QScrollArea;
class QLineEdit;
class FilteringProxyModel;
class FilterStrings;
class FilterDates;
class FilterNumbers;
class TableModel;
class QVBoxLayout;

/**
 * @brief Dock widget with filters for each opened dataset.
 */
class FiltersDock : public Dock
{
    Q_OBJECT
public:
    explicit FiltersDock(QWidget* parent = nullptr);

    void addFiltersForModel(const FilteringProxyModel* model);

    void removeFiltersForModel(const FilteringProxyModel* model);

    void showFiltersForModel(const FilteringProxyModel* model);

private:
    FilterStrings* createStringsFilter(const TableModel* parentModel,
                                       int index);

    FilterDates* createDatesFilter(const TableModel* parentModel, int index);

    FilterNumbers* createNumbersFilter(const TableModel* parentModel,
                                       int index);

    QWidget* createFiltersWidgets(const FilteringProxyModel* model);

    static QString getColumnName(const TableModel* parentModel, int index);

    QLineEdit* createSearchLineEdit(QWidget* parent) const;

    QScrollArea* createScrollAreaWithFilters(const FilteringProxyModel* model,
                                             QWidget* parent);

    void fillLayoutWithFilterWidgets(QVBoxLayout* layout,
                                     const FilteringProxyModel* model);

    QMap<const FilteringProxyModel*, QWidget*> modelsMap_;

    QStackedWidget stackedWidget_;

private Q_SLOTS:
    void searchTextChanged(const QString& arg1);

Q_SIGNALS:
    void filterNumbers(int column, double from, double to);

    void filterNames(int column, QStringList exclusionList);

    void filterDates(int column, QDate from, QDate to, bool filterEmptyDates);
};
