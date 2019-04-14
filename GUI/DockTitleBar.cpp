#include <QPainter>
#include <QDebug>
#include <QStyle>

#include "Common/Constants.h"

#include "DockTitleBar.h"
#include "ui_DockTitleBar.h"

DockTitleBar::DockTitleBar(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DockTitleBar)
{
    ui->setupUi(this);
    ui->reset->setVisible(false);
    ui->selectAll->setVisible(false);
    ui->unselectAll->setVisible(false);
    ui->exportAll->setVisible(false);
    ui->floating->setVisible(true);

    ui->close->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    ui->reset->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
}

DockTitleBar::~DockTitleBar()
{
    delete ui;
}

QPushButton* DockTitleBar::getCloseButton()
{
    return ui->close;
}

QPushButton* DockTitleBar::getFloatingButton()
{
    return ui->floating;
}

QPushButton* DockTitleBar::getSelectAllButton()
{
    return ui->selectAll;
}

QPushButton* DockTitleBar::getUnselectAllButton()
{
    return ui->unselectAll;
}

QPushButton* DockTitleBar::getExportButton()
{
    return ui->exportAll;
}

QPushButton* DockTitleBar::getResetButton()
{
    return ui->reset;
}

void DockTitleBar::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setPen(Qt::darkGray);
    QRect rectangle = rect();
    rectangle.setHeight(rectangle.height() - 1);
    rectangle.setWidth(rectangle.width() - 1);
    painter.drawRect(rectangle);
}

void DockTitleBar::setTitle(const QString& titleText)
{
    ui->label->setText(titleText);
}
