#include <QDebug>
#include <QScrollArea>
#include <QLineEdit>

#include "ModelsAndViews/TableModel.h"
#include "ModelsAndViews/FilteringProxyModel.h"
#include "Common/Constants.h"

#include "FiltersDock.h"
#include "ui_FiltersDock.h"
#include "FilterNumbers.h"
#include "FilterDates.h"
#include "FilterNames.h"
#include "Filter.h"

FiltersDock::FiltersDock(QWidget* parent) :
    DockWidget(QObject::tr("Filters"), parent),
    ui(new Ui::FiltersDock)
{
    ui->setupUi(this);
}

FiltersDock::~FiltersDock()
{
    delete ui;
}

void FiltersDock::addModel(const FilteringProxyModel* model)
{
    if (nullptr == model)
    {
        return;
    }
    auto mainWidget = new QWidget();
    modelsMap_[mainWidget] = model;

    auto mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    //Search line edit.
    auto lineEdit = new QLineEdit(mainWidget);
    lineEdit->setPlaceholderText(tr("Search..."));
    lineEdit->setClearButtonEnabled(true);
    connect(lineEdit,
            SIGNAL(textChanged(QString)),
            this,
            SLOT(searchTextChanged(QString)));
    mainLayout->addWidget(lineEdit);

    auto filterListWidget = new QWidget();

    //Create layout.
    auto filterListLayout = new QVBoxLayout(filterListWidget);
    filterListLayout->setContentsMargins(0, 0, 0, 0);
    filterListLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    filterListWidget->setLayout(filterListLayout);
    filterListLayout->addStretch();

    //Add filter widgets for each column.
    const TableModel* parentModel = model->getParentModel();
    createFiltersWidgets(parentModel, filterListWidget, filterListLayout);

    //Add scroll area for filterListWidget.
    auto scrollArea = new QScrollArea();
    scrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    scrollArea->setWidget(filterListWidget);
    scrollArea->setWidgetResizable(true);

    //Add scroll area to main layout.
    mainLayout->addWidget(scrollArea);

    ui->stackedWidget->addWidget(mainWidget);
    activateFiltersForModel(model);
}

void FiltersDock::createFiltersWidgets(const TableModel* model,
                                       QWidget* filterListWidget,
                                       QVBoxLayout* layout)
{
    for (int i = 0; i < model->columnCount(); ++i)
    {
        Filter* filter = nullptr;
        switch (model->getColumnFormat(i))
        {
            case DATA_FORMAT_STRING:
            {
                filter = createNewNamesFilter(model, i, filterListWidget);

                break;
            }
            case DATA_FORMAT_DATE:
            {
                filter = createNewDatesFilter(model, i, filterListWidget);

                break;
            }
            case DATA_FORMAT_FLOAT:
            {
                filter = createNewNumbersFilter(model, i, filterListWidget);

                break;
            }
            default:
            {
                Q_ASSERT(false);
            }
        }
        layout->addWidget(filter);
    }
}

FilterNames* FiltersDock::createNewNamesFilter(const TableModel* parentModel,
                                               int index,
                                               QWidget* filterListWidget)
{
    QString columnName = parentModel->headerData(index, Qt::Horizontal).toString();
    QStringList list;
    parentModel->getStringList(index, list);
    auto filter = new FilterNames(columnName, index, list, filterListWidget);

    connect(filter,
            SIGNAL(newStringFilter(int, QSet<QString>)),
            this,
            SIGNAL(newNamesFiltering(int, QSet<QString>)));

    if (1 >= list.size())
    {
        filter->setChecked(false);
    }

    return filter;
}

FilterDates* FiltersDock::createNewDatesFilter(const TableModel* parentModel,
                                               int index,
                                               QWidget* filterListWidget)
{
    QString columnName = parentModel->headerData(index, Qt::Horizontal).toString();
    QDate min, max;
    bool emptyDates = false;
    parentModel->getDateRange(index, min, max, emptyDates);
    auto filter = new FilterDates(columnName,
                                  index,
                                  min,
                                  max,
                                  emptyDates,
                                  filterListWidget);

    connect(filter,
            SIGNAL(newDateFilter(int, QDate, QDate, bool)),
            this,
            SIGNAL(newDateFiltering(int, QDate, QDate, bool)));

    return filter;
}

FilterNumbers* FiltersDock::createNewNumbersFilter(const TableModel* parentModel,
                                                   int index,
                                                   QWidget* filterListWidget)
{
    QString columnName = parentModel->headerData(index, Qt::Horizontal).toString();
    double min = -1, max = -1;
    parentModel->getNumericRange(index, min, max);
    auto filter = new FilterNumbers(columnName,
                                    index,
                                    min,
                                    max,
                                    filterListWidget);

    connect(filter,
            SIGNAL(newNumericFilter(int, double, double)),
            this,
            SIGNAL(newNumbersFiltering(int, double, double)));

    return filter;
}

void FiltersDock::removeModel(const FilteringProxyModel* model)
{
    if (nullptr != model)
    {
        QWidget* widgetWithFiltersToDelete = modelsMap_.key(model);
        modelsMap_.remove(widgetWithFiltersToDelete);
        ui->stackedWidget->removeWidget(widgetWithFiltersToDelete);
        delete widgetWithFiltersToDelete;
    }
}

void FiltersDock::activateFiltersForModel(const FilteringProxyModel* model)
{
    if (nullptr != model)
    {
        ui->stackedWidget->setCurrentWidget(modelsMap_.key(model));
    }
}

void FiltersDock::searchTextChanged(const QString& arg1)
{
    QWidget* currentWidget = ui->stackedWidget->currentWidget();

    if (nullptr == currentWidget)
    {
        Q_ASSERT(false);
        return;
    }

    QList<Filter*> widgets = currentWidget->findChildren<Filter*>();

    for (Filter* current : widgets)
    {
        if (nullptr != current)
        {
            current->setVisible(current->title().contains(arg1, Qt::CaseInsensitive));
        }
        else
        {
            Q_ASSERT(false);
        }
    }
}
