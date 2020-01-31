#include "FilterNumbers.h"

#include <cmath>

#include <DoubleSlider.h>
#include <QDebug>
#include <QIntValidator>

#include "Common/Constants.h"

#include "ui_FilterNumbers.h"

FilterNumbers::FilterNumbers(const QString& name,
                             int column,
                             double min,
                             double max,
                             QWidget* parent) :
    Filter(name, column, parent),
    ui(new Ui::FilterNumbers),
    minOnInit_(min),
    maxOnInit_(max)
{
    ui->setupUi(this);

    connect(this, &Filter::toggled, this, &FilterNumbers::setChecked);

    if (maxOnInit_ - minOnInit_ <= 1)
    {
        doubleMode_ = true;
    }

    if (!doubleMode_)
    {
        if (fmod(minOnInit_, 1) != 0 && minOnInit_ < 0)
        {
            minOnInit_ = minOnInit_ - 1;
        }

        if (fmod(maxOnInit_, 1) != 0 && maxOnInit_ > 0)
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

    DoubleSlider* slider {nullptr};

    if (doubleMode_)
    {
        slider = new DoubleSlider(static_cast<int>(ui->fromValue->text().toDouble() * FACTOR),
                                  static_cast<int>(ui->toValue->text().toDouble() * FACTOR),
                                  this);
    }
    else
    {
        slider = new DoubleSlider(static_cast<int>(minOnInit_), static_cast<int>(maxOnInit_), this);
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
        ui->fromValue->setValidator(new QIntValidator(static_cast<int>(minOnInit_),
                                                      static_cast<int>(maxOnInit_),
                                                      ui->fromValue));
        ui->toValue->setValidator(new QIntValidator(static_cast<int>(minOnInit_),
                                                    static_cast<int>(maxOnInit_),
                                                    ui->toValue));
    }

    connect(slider, &DoubleSlider::minChanged, this, &FilterNumbers::sliderMinChanged);
    connect(slider, &DoubleSlider::maxChanged, this, &FilterNumbers::sliderMaxChanged);

    connect(ui->fromValue, &QLineEdit::editingFinished, this, &FilterNumbers::fromEditingFinished);
    connect(ui->toValue, &QLineEdit::editingFinished, this, &FilterNumbers::toEditingFinished);

    ui->verticalLayout->addWidget(slider);

    if (Constants::doublesAreEqual(ui->fromValue->text().toDouble(), ui->toValue->text().toDouble()))
    {
        setDisabled(true);
    }
}

FilterNumbers::~FilterNumbers()
{
    delete ui;
}

void FilterNumbers::sliderMinChanged(int newValue)
{
    if (doubleMode_)
    {
        ui->fromValue->setText(QString::number(newValue / FACTOR, 'f', 2));
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
        ui->toValue->setText(QString::number(newValue / FACTOR, 'f', 2));
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
    auto slider = findChild<DoubleSlider*>();
    if (slider == nullptr)
    {
        return;
    }

    QString newMinAsText = ui->fromValue->text();
    double minToSet {doubleMode_ ?
                     newMinAsText.toDouble()* FACTOR :
                     newMinAsText.toInt()};
    slider->setCurrentMin(minToSet);
}

void FilterNumbers::toEditingFinished()
{
    auto slider = findChild<DoubleSlider*>();
    if (slider == nullptr)
    {
        return;
    }

    QString newMaxAsText = ui->toValue->text();
    double maxToSet {doubleMode_ ?
                     newMaxAsText.toDouble()* FACTOR :
                     newMaxAsText.toInt()};
    slider->setCurrentMax(maxToSet);
}

void FilterNumbers::setChecked(bool checked)
{
    QGroupBox::setChecked(checked);
    QList<QWidget*> widgets = findChildren<QWidget*>();

    for (QWidget* current : widgets)
    {
        current->setVisible(checked);
    }
}
