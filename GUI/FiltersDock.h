#pragma once

#include <QDate>
#include <QMap>
#include <QStackedWidget>

#include "GUI/Dock.h"

class FilteringProxyModel;
class FilterStrings;
class FilterDates;
class FilterNumbers;
class TableModel;
class QVBoxLayout;
class QStackedWidget;

/**
 * @brief Filters dock used as parent for filters widget created for each
 * dataset.
 */
class FiltersDock : public Dock
{
    Q_OBJECT
public:
    explicit FiltersDock(QWidget* parent = nullptr);

    ~FiltersDock() override = default;

    /// Adds new widget with filters for given model.
    void addModel(const FilteringProxyModel* model);

    /// Removes widget related to given model.
    void removeModel(const FilteringProxyModel* model);

    /// Shows widget with filters related with given model.
    void activateFiltersForModel(const FilteringProxyModel* model);

private:
    /// Create names filter for given model column and return it.
    FilterStrings* createNewStringsFilter(const TableModel* parentModel,
                                          int index, QWidget* filterListWidget);

    /// Create dates filter for given model column and return it.
    FilterDates* createNewDatesFilter(const TableModel* parentModel, int index,
                                      QWidget* filterListWidget);

    /// Create numbers filter for given model column and return it.
    FilterNumbers* createNewNumbersFilter(const TableModel* parentModel,
                                          int index, QWidget* filterListWidget);

    void createFiltersWidgets(const TableModel* model,
                              QWidget* filterListWidget, QVBoxLayout* layout);

    /// Used to find widget related with model.
    QMap<QWidget*, const FilteringProxyModel*> modelsMap_;

    QStackedWidget stackedWidget_;

private Q_SLOTS:
    void searchTextChanged(const QString& arg1);

Q_SIGNALS:
    void newNumbersFiltering(int column, double from, double to);

    void newNamesFiltering(int column, QStringList exclusionList);

    void newDateFiltering(int column, QDate from, QDate to,
                          bool filterEmptyDates);
};
