#include "FiltersDock.h"

#include <Filter.h>
#include <FilterDates.h>
#include <FilterDoubles.h>
#include <FilterStrings.h>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>

#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"

FiltersDock::FiltersDock(QWidget* parent)
    : Dock(QObject::tr("Filters"), parent), stackedWidget_(this)
{
    setWidget(&stackedWidget_);
}

void FiltersDock::addModel(const FilteringProxyModel* model)
{
    if (model == nullptr)
        return;

    auto mainWidget{new QWidget()};
    modelsMap_[mainWidget] = model;

    auto mainLayout{new QVBoxLayout(mainWidget)};
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(createSearchLineEdit(mainWidget));
    mainLayout->addWidget(createScrollAreaWithFilters(model, mainWidget));

    stackedWidget_.addWidget(mainWidget);
    activateFiltersForModel(model);
}

QWidget* FiltersDock::createFiltersWidgets(const FilteringProxyModel* model)
{
    const TableModel* parentModel{model->getParentModel()};

    auto filterListWidget{new QWidget()};
    auto layout{new QVBoxLayout(filterListWidget)};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    filterListWidget->setLayout(layout);
    layout->addStretch();

    for (int i = 0; i < model->columnCount(); ++i)
    {
        Filter* filter{nullptr};
        switch (parentModel->getColumnFormat(i))
        {
            case ColumnType::STRING:
            {
                filter =
                    createNewStringsFilter(parentModel, i, filterListWidget);
                break;
            }
            case ColumnType::DATE:
            {
                filter = createNewDatesFilter(parentModel, i, filterListWidget);
                break;
            }
            case ColumnType::NUMBER:
            {
                filter =
                    createNewNumbersFilter(parentModel, i, filterListWidget);
                break;
            }
            case ColumnType::UNKNOWN:
            {
                Q_ASSERT(false);
            }
        }
        layout->addWidget(filter);
    }

    return filterListWidget;
}

QString FiltersDock::getColumnName(const TableModel* parentModel,
                                   int index) const
{
    return parentModel->headerData(index, Qt::Horizontal).toString();
}

QLineEdit* FiltersDock::createSearchLineEdit(QWidget* parent)
{
    auto lineEdit{new QLineEdit(parent)};
    lineEdit->setPlaceholderText(tr("Search..."));
    lineEdit->setClearButtonEnabled(true);
    connect(lineEdit, &QLineEdit::textChanged, this,
            &FiltersDock::searchTextChanged);
    return lineEdit;
}

QScrollArea* FiltersDock::createScrollAreaWithFilters(
    const FilteringProxyModel* model, QWidget* parent)
{
    QWidget* filterListWidget{createFiltersWidgets(model)};

    auto scrollArea{new QScrollArea(parent)};
    scrollArea->setSizePolicy(
        QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    scrollArea->setWidget(filterListWidget);
    scrollArea->setWidgetResizable(true);

    return scrollArea;
}

FilterStrings* FiltersDock::createNewStringsFilter(
    const TableModel* parentModel, int index, QWidget* filterListWidget)
{
    const QString columnName{getColumnName(parentModel, index)};
    QStringList list{parentModel->getStringList(index)};
    const int itemCount{list.size()};
    list.sort();
    auto filter{
        new FilterStrings(columnName, std::move(list), filterListWidget)};
    auto emitChangeForColumn{[=](QStringList bannedList) {
        Q_EMIT newNamesFiltering(index, std::move(bannedList));
    }};
    connect(filter, &FilterStrings::newStringFilter, this, emitChangeForColumn);

    filter->setCheckable(true);
    if (itemCount <= 1)
        filter->setChecked(false);

    return filter;
}

FilterDates* FiltersDock::createNewDatesFilter(const TableModel* parentModel,
                                               int index,
                                               QWidget* filterListWidget)
{
    const QString columnName{getColumnName(parentModel, index)};
    const auto [minDate, maxDate,
                haveEmptyDates]{parentModel->getDateRange(index)};
    auto filter{new FilterDates(columnName, minDate, maxDate, haveEmptyDates,
                                filterListWidget)};
    auto emitChangeForColumn{[=](QDate from, QDate to, bool filterEmptyDates) {
        Q_EMIT newDateFiltering(index, from, to, filterEmptyDates);
    }};
    connect(filter, &FilterDates::newDateFilter, this, emitChangeForColumn);
    filter->setCheckable(true);
    return filter;
}

FilterNumbers* FiltersDock::createNewNumbersFilter(
    const TableModel* parentModel, int index, QWidget* filterListWidget)
{
    const QString columnName{getColumnName(parentModel, index)};
    const auto [min, max]{parentModel->getNumericRange(index)};
    auto filter{new FilterDoubles(columnName, min, max, filterListWidget)};
    auto emitChangeForColumn{[=](double from, double to) {
        Q_EMIT newNumbersFiltering(index, from, to);
    }};
    connect(filter, &FilterDoubles::newNumericFilter, this,
            emitChangeForColumn);
    filter->setCheckable(true);
    return filter;
}

void FiltersDock::removeModel(const FilteringProxyModel* model)
{
    if (model == nullptr)
        return;

    QWidget* widgetToDelete{modelsMap_.key(model)};
    modelsMap_.remove(widgetToDelete);
    stackedWidget_.removeWidget(widgetToDelete);
    delete widgetToDelete;
}

void FiltersDock::activateFiltersForModel(const FilteringProxyModel* model)
{
    if (model != nullptr)
        stackedWidget_.setCurrentWidget(modelsMap_.key(model));
}

void FiltersDock::searchTextChanged(const QString& arg1)
{
    QWidget* currentWidget{stackedWidget_.currentWidget()};
    for (auto filter : currentWidget->findChildren<Filter*>())
        filter->setVisible(filter->title().contains(arg1, Qt::CaseInsensitive));
}
