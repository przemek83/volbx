#include "TabBar.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QMainWindow>
#include <QRegExpValidator>
#include <QTabWidget>

#include "Common/Constants.h"

TabBar::TabBar(QWidget* parent) :
    QTabBar(parent)
{
    lineEdit_.setParent(this);
    lineEdit_.setValidator(new QRegExpValidator(QRegExp(Constants::datasetNameRegExp), this));
    lineEdit_.hide();

    connect(&lineEdit_, &QLineEdit::editingFinished,
            this, &TabBar::editingOfnameFinished);

    lineEdit_.installEventFilter(this);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    int index = currentIndex();
    QRect tabRectangle = tabRect(index);
    lineEdit_.move(tabRectangle.x(), tabRectangle.y());
    lineEdit_.resize(tabRectangle.size());

    auto tabWidget = dynamic_cast<QTabWidget*>(parentWidget());
    auto mainWindow = dynamic_cast<QMainWindow*>(tabWidget->currentWidget());
    lineEdit_.setText(mainWindow->windowTitle());
    lineEdit_.show();
    lineEdit_.setFocus();

    QTabBar::mouseDoubleClickEvent(event);
}

void TabBar::editingOfnameFinished()
{
    if (lineEdit_.isVisible())
    {
        //Currently in eksport window is used name set as window title.
        int index = currentIndex();
        lineEdit_.hide();
        QString currentTabText = tabText(index);

        auto tabWidget = dynamic_cast<QTabWidget*>(parentWidget());
        auto mainWindow = dynamic_cast<QMainWindow*>(tabWidget->currentWidget());
        QString suffix = currentTabText.remove(0, mainWindow->windowTitle().length());
        setTabText(index, lineEdit_.text() + suffix);
        mainWindow->setWindowTitle(lineEdit_.text());
    }
}

bool TabBar::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (nullptr != keyEvent && Qt::Key_Escape == keyEvent->key())
        {
            lineEdit_.hide();
            return true;
        }
    }

    //Standard event processing
    return QObject::eventFilter(obj, event);
}
