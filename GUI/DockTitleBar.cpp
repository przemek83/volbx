#include "DockTitleBar.h"

#include <QPainter>
#include <QStyle>

#include "ui_DockTitleBar.h"

DockTitleBar::DockTitleBar(QWidget* parent)
    : QWidget(parent), ui(new Ui::DockTitleBar)
{
    ui->setupUi(this);
    ui->reset->setVisible(false);
    ui->selectAll->setVisible(false);
    ui->unselectAll->setVisible(false);
    ui->exportAll->setVisible(false);
    ui->floating->setVisible(true);

    ui->close->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    ui->reset->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_BrowserReload));

    connectButtons();
}

DockTitleBar::~DockTitleBar() { delete ui; }

void DockTitleBar::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    drawBorder();
}

void DockTitleBar::connectButtons()
{
    connect(ui->close, &QPushButton::clicked, this,
            &DockTitleBar::closeClicked);
    connect(ui->floating, &QPushButton::clicked, this,
            &DockTitleBar::floatingClicked);
    connect(ui->selectAll, &QPushButton::clicked, this,
            &DockTitleBar::selectAllClicked);
    connect(ui->unselectAll, &QPushButton::clicked, this,
            &DockTitleBar::unselectAllClicked);
    connect(ui->exportAll, &QPushButton::clicked, this,
            &DockTitleBar::exportClicked);
    connect(ui->reset, &QPushButton::clicked, this,
            &DockTitleBar::resetClicked);
}

void DockTitleBar::drawBorder()
{
    QPainter painter(this);
    painter.setPen(Qt::darkGray);
    QRect rectangle{rect()};
    rectangle.setHeight(rectangle.height() - 1);
    rectangle.setWidth(rectangle.width() - 1);
    painter.drawRect(rectangle);
}

QPushButton* DockTitleBar::getButton(DockTitleBar::Button button) const
{
    QPushButton* pushButton{nullptr};
    switch (button)
    {
        case Button::CLOSE:
            pushButton = ui->close;
            break;
        case Button::FLOATING:
            pushButton = ui->floating;
            break;
        case Button::SELECT_ALL:
            pushButton = ui->selectAll;
            break;
        case Button::UNSELECT_ALL:
            pushButton = ui->unselectAll;
            break;
        case Button::EXPORT:
            pushButton = ui->exportAll;
            break;
        case Button::RESET:
            pushButton = ui->reset;
            break;
    }
    return pushButton;
}

void DockTitleBar::setTitle(const QString& titleText)
{
    ui->label->setText(titleText);
}

void DockTitleBar::setButtonVisible(DockTitleBar::Button button, bool visible)
{
    getButton(button)->setVisible(visible);
}

void DockTitleBar::setButtonEnabled(DockTitleBar::Button button, bool enabled)
{
    getButton(button)->setEnabled(enabled);
}
