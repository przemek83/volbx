#ifndef FILTERSDOCK_H
#define FILTERSDOCK_H

#include <QMap>
#include <QDate>

#include "GUI/DockWidget.h"

class FilteringProxyModel;
class FilterNames;
class FilterDates;
class FilterNumbers;
class TableModel;
class QVBoxLayout;

namespace Ui {
    class FiltersDock;
}

/**
 * @brief Filters dock used as parent for filters widget created for each dataset.
 */
class FiltersDock : public DockWidget
{
    Q_OBJECT
public:
    explicit FiltersDock(QWidget *parent = nullptr);

    ~FiltersDock() override;

    ///Adds new widget with filters for given model.
    void addModel(const FilteringProxyModel* model);

    ///Removes widget related to given model.
    void removeModel(const FilteringProxyModel *model);

    ///Shows widget with filters related with given model.
    void activateFiltersForModel(const FilteringProxyModel* model);

private slots:
    void searchTextChanged(const QString arg1);

private:
    Q_DISABLE_COPY(FiltersDock)

    Ui::FiltersDock *ui;

    ///Used to find widget related with model.
    QMap<QWidget*, const FilteringProxyModel*> modelsMap_;

    ///Create names filter for given model column and return it.
    FilterNames* createNewNamesFilter(const TableModel* parentModel,
                                      int index,
                                      QWidget* filterListWidget);

    ///Create dates filter for given model column and return it.
    FilterDates* createNewDatesFilter(const TableModel* parentModel,
                                      int index,
                                      QWidget* filterListWidget);

    ///Create numbers filter for given model column and return it.
    FilterNumbers* createNewNumbersFilter(const TableModel* parentModel,
                                          int index,
                                          QWidget* filterListWidget);

    void createFiltersWidgets(const TableModel* model,
                              QWidget* filterListWidget,
                              QVBoxLayout* layout);

signals:
    void newNumbersFiltering(int column, double from, double to);

    void newNamesFiltering(int column, QSet<QString> exclusionList);

    void newDateFiltering(int column, QDate from, QDate to, bool filterEmptyDates);
};

#endif // FILTERSDOCK_H
