#include "FilterDates.h"

#include <QDebug>

#include "Common/Constants.h"

#include "ui_FilterDates.h"

FilterDates::FilterDates(const QString& name,
                         int column,
                         QDate min,
                         QDate max,
                         bool emptyDates,
                         QWidget* parent) :
    Filter(name, column, parent),
    minOnInit_(min),
    maxOnInit_(max),
    ui(new Ui::FilterDates),
    emptyDates_(emptyDates)
{
    ui->setupUi(this);

    connect(this, &Filter::toggled, this, &FilterDates::setChecked);
    connect(ui->emptyDates, &QCheckBox::toggled,
            this, &FilterDates::emptyDatesToggled);

    if (minOnInit_ == maxOnInit_)
    {
        setDisabled(true);
    }

    //Lower.
    calendarLeft_.setFirstDayOfWeek(Qt::Monday);
    calendarLeft_.setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->lowerDateEdit->setDate(minOnInit_);
    ui->lowerDateEdit->setCalendarWidget(&calendarLeft_);
    connect(ui->lowerDateEdit, &QDateEdit::dateChanged,
            this, &FilterDates::lowerDateChanged);

    //Higher.
    calendarRight_.setFirstDayOfWeek(Qt::Monday);
    calendarRight_.setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->higherDateEdit->setDate(maxOnInit_);
    ui->higherDateEdit->setCalendarWidget(&calendarRight_);
    connect(ui->higherDateEdit, &QDateEdit::dateChanged,
            this, &FilterDates::higherDateChanged);

    if (!emptyDates_)
    {
        ui->emptyDates->hide();
    }
    else
    {
        ui->emptyDates->show();
    }
}

FilterDates::~FilterDates()
{
    delete ui;
}

void FilterDates::lowerDateChanged(QDate newDate)
{
    //signal block?
    if (newDate > ui->higherDateEdit->date())
    {
        ui->higherDateEdit->setDate(newDate);
    }

    QApplication::processEvents();

    //Emit to model via filters dock.
    Q_EMIT newDateFilter(column_, ui->lowerDateEdit->date(),
                         ui->higherDateEdit->date(),
                         ui->emptyDates->isChecked());
}

void FilterDates::higherDateChanged(QDate newDate)
{
    //signal block?
    if (newDate < ui->lowerDateEdit->date())
    {
        ui->lowerDateEdit->setDate(newDate);
    }

    QApplication::processEvents();

    //Emit to model via filters dock.
    Q_EMIT newDateFilter(column_, ui->lowerDateEdit->date(),
                         ui->higherDateEdit->date(), ui->emptyDates->isChecked());
}

void FilterDates::setChecked(bool checked)
{
    QGroupBox::setChecked(checked);

    QList<QDateEdit*> dateWidgets = findChildren<QDateEdit*>();

    for (QWidget* current : dateWidgets)
    {
        current->setVisible(checked);
    }

    ui->emptyDates->setVisible(checked && emptyDates_);
}

void FilterDates::emptyDatesToggled(bool checked)
{
    QApplication::processEvents();
    Q_EMIT newDateFilter(column_,
                         ui->lowerDateEdit->date(),
                         ui->higherDateEdit->date(),
                         checked);
}
