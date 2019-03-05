#include <algorithm>

#include <QListWidgetItem>
#include <QDebug>
#include <QScrollBar>

#include "Common/Constants.h"

#include "FilterNames.h"
#include "ui_FilterNames.h"

const int FilterNames::minNameWidthForScrollMargin_ = 25;

const int FilterNames::maximumHeigh_ = 180;

FilterNames::FilterNames(QString name,
                         int column,
                         const QStringList& initialList,
                         QWidget* parent) :
    Filter(name, column, parent),
    initialList_(initialList),
    lastEmitted_(QSet<QString>()),
    ui(new Ui::FilterNames),
    addMarginForScrollBar_(false)
{
    ui->setupUi(this);

    initialList_.sort();

    int longestNameWidth = 0;
    for (const QString& itemName : initialList_)
    {
        auto item = new QListWidgetItem(itemName, ui->listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        longestNameWidth = qMax(longestNameWidth, itemName.length());
    }

    if (minNameWidthForScrollMargin_ <= longestNameWidth)
    {
        addMarginForScrollBar_ = true;
    }

    connect(ui->listWidget,
            SIGNAL(itemClicked(QListWidgetItem*)),
            this,
            SLOT(itemChecked(QListWidgetItem*)));

    if (initialList_.size() <= 1)
    {
        ui->selectAll->hide();
    }
}

FilterNames::~FilterNames()
{
    delete ui;
}

void FilterNames::itemChecked(QListWidgetItem* item)
{
    if (!item)
    {
        return;
    }

    QSet<QString> currentList;
    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* currentItem = ui->listWidget->item(i);
        if (Qt::Unchecked == currentItem->checkState())
        {
            currentList << currentItem->text();
        }
    }

    //If sizes are same it means nothing happen lately.
    if (currentList.count() == lastEmitted_.count())
    {
        return;
    }

    lastEmitted_ = currentList;

    bool allChecked = true;
    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        if (Qt::Unchecked == ui->listWidget->item(i)->checkState())
        {
            allChecked = false;
            break;
        }
    }
    ui->selectAll->blockSignals(true);
    ui->selectAll->setCheckState((allChecked ? Qt::Checked : Qt::Unchecked));
    ui->selectAll->blockSignals(false);

    QApplication::processEvents();

    Q_EMIT newStringFilter(column_, lastEmitted_);
}

QSize FilterNames::sizeHint() const
{
    if (true == isChecked())
    {
        int maxListHeight =
            std::min(ui->listWidget->sizeHintForRow(0) *
                     ui->listWidget->count() + 2 * ui->listWidget->frameWidth(),
                     maximumHeigh_);

        /* Add space for scroll in case of 3 or less items and long
           names detected in constructor.*/
        if (true == addMarginForScrollBar_ && 3 >= ui->listWidget->count())
        {
            //Scroll size retrieved here is not actual one, use rtow heigh instead.
            maxListHeight += ui->listWidget->sizeHintForRow(0);
        }

        ui->listWidget->setMaximumHeight(maxListHeight);
    }
    return QGroupBox::sizeHint();
}

void FilterNames::setChecked(bool checked)
{
    QGroupBox::setChecked(checked);

    auto checkBox = findChild<QCheckBox*>();

    QList<QWidget*> widgets = findChildren<QWidget*>();
    widgets.removeOne(checkBox);

    for (QWidget* current : widgets)
    {
        current->setVisible(checked);
    }

    checkBox->setVisible(true == checked && initialList_.size() > 1);
}

void FilterNames::on_selectAll_toggled(bool checked)
{
    Q_ASSERT(ui->listWidget->count() > 0);

    ui->listWidget->blockSignals(true);
    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        if (checked)
        {
            ui->listWidget->item(i)->setCheckState(Qt::Checked);
        }
        else
        {
            ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
        }
    }
    ui->listWidget->blockSignals(false);
    itemChecked(ui->listWidget->item(0));
}
