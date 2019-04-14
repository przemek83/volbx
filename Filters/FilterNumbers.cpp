#include <math.h>

#include <QDebug>
#include <QIntValidator>

#include "Common/Constants.h"
#include "GUI/DoubleSlider.h"

#include "FilterNumbers.h"
#include "ui_FilterNumbers.h"

const int FilterNumbers::factor_ = 100;

FilterNumbers::FilterNumbers(const QString& name,
                             int column,
                             double min,
                             double max,
                             QWidget* parent) :
    Filter(name, column, parent),
    minOnInit_(min),
    maxOnInit_(max),
    doubleMode_(false),
    ui(new Ui::FilterNumbers)
{
    ui->setupUi(this);

    if (maxOnInit_ - minOnInit_ <= 1)
    {
        doubleMode_ = true;
    }

    if (!doubleMode_)
    {
        if (fmod(minOnInit_, 1) && minOnInit_ < 0)
        {
            minOnInit_ = minOnInit_ - 1;
        }

        if (fmod(maxOnInit_, 1) && maxOnInit_ > 0)
        {
            maxOnInit_ = maxOnInit_ + 1;
        }

        minOnInit_ = static_cast<int>(minOnInit_);
        maxOnInit_ = static_cast<int>(maxOnInit_);
    }

    if (doubleMode_)
    {
        ui->fromValue->setText(QString::number(minOnInit_, 'f', 2));
        ui->toValue->setText(QString::number(maxOnInit_, 'f', 2));
    }
    else
    {
        ui->fromValue->setText(QString::number(minOnInit_));
        ui->toValue->setText(QString::number(maxOnInit_));
    }

    if (doubleMode_)
    {
        slider_ = new DoubleSlider(ui->fromValue->text().toDouble() * factor_,
                                   ui->toValue->text().toDouble() * factor_,
                                   this);
    }
    else
    {
        slider_ = new DoubleSlider(minOnInit_, maxOnInit_, this);
    }

    if (doubleMode_)
    {
        ui->fromValue->setValidator(new QDoubleValidator(minOnInit_,
                                                         maxOnInit_,
                                                         2,
                                                         ui->fromValue));
        ui->toValue->setValidator(new QDoubleValidator(minOnInit_,
                                                       maxOnInit_,
                                                       2,
                                                       ui->toValue));
    }
    else
    {
        ui->fromValue->setValidator(new QIntValidator(minOnInit_,
                                                      maxOnInit_,
                                                      ui->fromValue));
        ui->toValue->setValidator(new QIntValidator(minOnInit_,
                                                    maxOnInit_,
                                                    ui->toValue));
    }

    connect(slider_, SIGNAL(minChanged(int)), this, SLOT(sliderMinChanged(int)));
    connect(slider_, SIGNAL(maxChanged(int)), this, SLOT(sliderMaxChanged(int)));

    connect(ui->fromValue, SIGNAL(editingFinished()), this, SLOT(fromEditingFinished()));
    connect(ui->toValue, SIGNAL(editingFinished()), this, SLOT(toEditingFinished()));

    ui->verticalLayout->addWidget(slider_);

    if (ui->fromValue->text().toDouble() == ui->toValue->text().toDouble())
    {
        setDisabled(true);
    }
}

FilterNumbers::~FilterNumbers()
{
    delete ui;
    delete slider_;
}

void FilterNumbers::sliderMinChanged(int newValue)
{
    if (doubleMode_)
    {
        ui->fromValue->setText(QString::number(newValue / static_cast<double>(factor_), 'f', 2));
    }
    else
    {
        ui->fromValue->setText(QString::number(newValue));
    }

    QApplication::processEvents();

    //Emit to model via filters dock.
    Q_EMIT newNumericFilter(column_, ui->fromValue->text().toDouble(),
                            ui->toValue->text().toDouble());
}

void FilterNumbers::sliderMaxChanged(int newValue)
{
    if (doubleMode_)
    {
        ui->toValue->setText(QString::number(newValue / static_cast<double>(factor_), 'f', 2));
    }
    else
    {
        ui->toValue->setText(QString::number(newValue));
    }

    QApplication::processEvents();

    //Emit to model via filters dock.
    Q_EMIT newNumericFilter(column_,
                            ui->fromValue->text().toDouble(),
                            ui->toValue->text().toDouble());
}

void FilterNumbers::fromEditingFinished()
{
    if (doubleMode_)
    {
        slider_->setCurrentMin(ui->fromValue->text().toDouble() * factor_);
    }
    else
    {
        slider_->setCurrentMin(ui->fromValue->text().toInt());
    }
}

void FilterNumbers::toEditingFinished()
{
    if (doubleMode_)
    {
        slider_->setCurrentMax(ui->toValue->text().toDouble() * factor_);
    }
    else
    {
        slider_->setCurrentMax(ui->toValue->text().toInt());
    }
}
