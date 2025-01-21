#include "DockTitleBar.h"

#include <QPainter>
#include <QStyle>

DockTitleBar::DockTitleBar() : QWidget()
{
    ui_->setupUi(this);
    ui_->reset->setVisible(false);
    ui_->selectAll->setVisible(false);
    ui_->unselectAll->setVisible(false);
    ui_->exportAll->setVisible(false);

    ui_->close->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    ui_->reset->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_BrowserReload));

    connectButtons();
}

void DockTitleBar::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    drawBorder();
}

void DockTitleBar::connectButtons() const
{
    connect(ui_->close, &QPushButton::clicked, this,
            &DockTitleBar::closeClicked);
    connect(ui_->floating, &QPushButton::clicked, this,
            &DockTitleBar::floatingClicked);
    connect(ui_->selectAll, &QPushButton::clicked, this,
            &DockTitleBar::selectAllClicked);
    connect(ui_->unselectAll, &QPushButton::clicked, this,
            &DockTitleBar::unselectAllClicked);
    connect(ui_->exportAll, &QPushButton::clicked, this,
            &DockTitleBar::exportClicked);
    connect(ui_->reset, &QPushButton::clicked, this,
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
            pushButton = ui_->close;
            break;
        case Button::FLOATING:
            pushButton = ui_->floating;
            break;
        case Button::SELECT_ALL:
            pushButton = ui_->selectAll;
            break;
        case Button::UNSELECT_ALL:
            pushButton = ui_->unselectAll;
            break;
        case Button::EXPORT:
            pushButton = ui_->exportAll;
            break;
        case Button::RESET:
            pushButton = ui_->reset;
            break;
        default:
            Q_ASSERT(false);
    }
    return pushButton;
}

void DockTitleBar::setTitle(const QString& titleText)
{
    ui_->label->setText(titleText);
}

void DockTitleBar::setButtonVisible(DockTitleBar::Button button,
                                    bool visible) const
{
    getButton(button)->setVisible(visible);
}

void DockTitleBar::setButtonEnabled(DockTitleBar::Button button,
                                    bool enabled) const
{
    getButton(button)->setEnabled(enabled);
}
