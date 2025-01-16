#include "FiltersDock.h"

#include <wble/Filter.h>
#include <wble/FilterDates.h>
#include <wble/FilterDoubles.h>
#include <wble/FilterStrings.h>
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

void FiltersDock::addFiltersForModel(const FilteringProxyModel* model)
{
    if (model == nullptr)
        return;

    auto* mainWidget{new QWidget()};
    modelsMap_[model] = mainWidget;

    auto* mainLayout{new QVBoxLayout(mainWidget)};
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(createSearchLineEdit(mainWidget));
    mainLayout->addWidget(createScrollAreaWithFilters(model, mainWidget));

    stackedWidget_.addWidget(mainWidget);
    showFiltersForModel(model);
}

QWidget* FiltersDock::createFiltersWidgets(const FilteringProxyModel* model)
{
    auto* filterListWidget{new QWidget()};
    auto* layout{new QVBoxLayout(filterListWidget)};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    filterListWidget->setLayout(layout);

    fillLayoutWithFilterWidgets(layout, model);

    layout->addStretch();

    return filterListWidget;
}

QString FiltersDock::getColumnName(const TableModel* parentModel, int index)
{
    return parentModel->headerData(index, Qt::Horizontal).toString();
}

QLineEdit* FiltersDock::createSearchLineEdit(QWidget* parent) const
{
    auto* lineEdit{new QLineEdit(parent)};
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

    auto* scrollArea{new QScrollArea(parent)};
    scrollArea->setSizePolicy(
        QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    scrollArea->setWidget(filterListWidget);
    scrollArea->setWidgetResizable(true);

    return scrollArea;
}

void FiltersDock::fillLayoutWithFilterWidgets(QVBoxLayout* layout,
                                              const FilteringProxyModel* model)
{
    const TableModel* parentModel{model->getParentModel()};
    for (int i{0}; i < model->columnCount(); ++i)
    {
        Filter* filter{nullptr};
        switch (parentModel->getColumnFormat(i))
        {
            case ColumnType::STRING:
            {
                filter = createStringsFilter(parentModel, i);
                break;
            }
            case ColumnType::DATE:
            {
                filter = createDatesFilter(parentModel, i);
                break;
            }
            case ColumnType::NUMBER:
            {
                filter = createNumbersFilter(parentModel, i);
                break;
            }
            case ColumnType::UNKNOWN:
            {
                Q_ASSERT(false);
            }
        }
        layout->addWidget(filter);
    }
}

FilterStrings* FiltersDock::createStringsFilter(const TableModel* parentModel,
                                                int index)
{
    const QString columnName{getColumnName(parentModel, index)};
    QStringList list{parentModel->getStringList(index)};
    const qsizetype itemCount{list.size()};
    list.sort();
    auto* filter{new FilterStrings(columnName, std::move(list))};
    auto emitChangeForColumn{[=](QStringList bannedList) {
        emit filterNames(index, std::move(bannedList));
    }};
    connect(filter, &FilterStrings::newStringFilter, this, emitChangeForColumn);

    filter->setCheckable(true);
    if (itemCount <= 1)
        filter->setChecked(false);

    return filter;
}

FilterDates* FiltersDock::createDatesFilter(const TableModel* parentModel,
                                            int index)
{
    const QString columnName{getColumnName(parentModel, index)};
    const auto [minDate, maxDate,
                haveEmptyDates]{parentModel->getDateRange(index)};
    auto* filter{new FilterDates(columnName, minDate, maxDate, haveEmptyDates)};
    auto emitChangeForColumn{[=](QDate from, QDate to, bool filterEmptyDates) {
        emit filterDates(index, from, to, filterEmptyDates);
    }};
    connect(filter, &FilterDates::newDateFilter, this, emitChangeForColumn);
    filter->setCheckable(true);
    return filter;
}

FilterNumbers* FiltersDock::createNumbersFilter(const TableModel* parentModel,
                                                int index)
{
    const QString columnName{getColumnName(parentModel, index)};
    const auto [min, max]{parentModel->getNumericRange(index)};
    auto* filter{new FilterDoubles(columnName, min, max)};
    auto emitChangeForColumn{[=](double from, double to)
                             { emit filterNumbers(index, from, to); }};
    connect(filter, &FilterDoubles::newNumericFilter, this,
            emitChangeForColumn);
    filter->setCheckable(true);
    return filter;
}

void FiltersDock::removeFiltersForModel(const FilteringProxyModel* model)
{
    if (model == nullptr)
        return;

    QWidget* widgetToDeleteRawPtr{modelsMap_.take(model)};
    const std::unique_ptr<QWidget> widgetToDelete{widgetToDeleteRawPtr};
    stackedWidget_.removeWidget(widgetToDeleteRawPtr);
}

void FiltersDock::showFiltersForModel(const FilteringProxyModel* model)
{
    if (model != nullptr)
        stackedWidget_.setCurrentWidget(modelsMap_.value(model));
}

void FiltersDock::searchTextChanged(const QString& arg1) const
{
    const QWidget* currentWidget{stackedWidget_.currentWidget()};
    const QList<Filter*> filters{currentWidget->findChildren<Filter*>()};
    for (auto* filter : filters)
        filter->setVisible(filter->title().contains(arg1, Qt::CaseInsensitive));
}
