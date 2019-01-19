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

FiltersDock::FiltersDock(QWidget *parent) :
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
    if( NULL == model )
    {
        return;
    }
    QWidget* mainWidget = new QWidget();
    modelsMap_[mainWidget] = model;

    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0,0,0,0);

    //Search line edit.
    QLineEdit* lineEdit = new QLineEdit(mainWidget);
    lineEdit->setPlaceholderText(tr("Search..."));
    lineEdit->setClearButtonEnabled(true);
    connect(lineEdit,
            SIGNAL(textChanged(QString)),
            this,
            SLOT(searchTextChanged(QString)));
    mainLayout->addWidget(lineEdit);

    QWidget* filterListWidget = new QWidget();

    //Create layout.
    QVBoxLayout* filterListLayout = new QVBoxLayout(filterListWidget);
    filterListLayout->setContentsMargins(0,0,0,0);
    filterListLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    filterListWidget->setLayout(filterListLayout);
    filterListLayout->addStretch();

    //Add filter widgets for each column.
    const TableModel* parentModel = model->getParentModel();
    createFiltersWidgets(parentModel, filterListWidget, filterListLayout);

    //Add scroll area for filterListWidget.
    QScrollArea* scrollArea = new QScrollArea();
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
    for(int i = 0; i < model->columnCount(); ++i)
    {
        Filter* filter = NULL;
        switch( model->getColumnFormat(i) )
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

FilterNames* FiltersDock::createNewNamesFilter(const TableModel *parentModel,
                                               int index,
                                               QWidget* filterListWidget)
{
    QString columnName = parentModel->headerData(index, Qt::Horizontal).toString();
    QStringList list;
    parentModel->getStringList(index, list);
    FilterNames* filter = new FilterNames(columnName, index, list, filterListWidget);

    connect(filter,
            SIGNAL(newStringFilter(int, QSet<QString>)),
            this,
            SIGNAL(newNamesFiltering(int, QSet<QString>)));

    if ( 1 >= list.size() )
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
    FilterDates* filter = new FilterDates(columnName,
                                          index,
                                          min,
                                          max,
                                          emptyDates,
                                          filterListWidget);

    connect(filter,
            SIGNAL(newDateFilter(int,QDate,QDate,bool)),
            this,
            SIGNAL(newDateFiltering(int,QDate,QDate,bool)));

    return filter;
}

FilterNumbers* FiltersDock::createNewNumbersFilter(const TableModel *parentModel,
                                                   int index,
                                                   QWidget* filterListWidget)
{
    QString columnName = parentModel->headerData(index, Qt::Horizontal).toString();
    double min = -1, max = -1;
    parentModel->getNumericRange(index, min, max);
    FilterNumbers* filter = new FilterNumbers(columnName,
                                              index,
                                              min,
                                              max,
                                              filterListWidget);

    connect(filter,
            SIGNAL(newNumericFilter(int,double,double)),
            this,
            SIGNAL(newNumbersFiltering(int,double,double)));

    return filter;
}

void FiltersDock::removeModel(const FilteringProxyModel* model)
{
    if( NULL != model )
    {
        QWidget* widgetWithFiltersToDelete = modelsMap_.key(model);
        modelsMap_.remove(widgetWithFiltersToDelete);
        ui->stackedWidget->removeWidget(widgetWithFiltersToDelete);
        delete widgetWithFiltersToDelete;
    }
}

void FiltersDock::activateFiltersForModel(const FilteringProxyModel* model)
{
    if( NULL != model )
    {
        ui->stackedWidget->setCurrentWidget(modelsMap_.key(model));
    }
}

void FiltersDock::searchTextChanged(const QString arg1)
{
    QWidget* currentWidget = ui->stackedWidget->currentWidget();

    if ( NULL == currentWidget )
    {
        Q_ASSERT(false);
        return;
    }

    QList<Filter*> widgets = currentWidget->findChildren<Filter*>();

    foreach(Filter* current, widgets)
    {
        if ( NULL != current )
        {
            current->setVisible(current->title().contains(arg1, Qt::CaseInsensitive));
        }
        else
        {
            Q_ASSERT(false);
        }
    }
}
