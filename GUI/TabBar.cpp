#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QMainWindow>
#include <QTabWidget>
#include <QRegExpValidator>

#include "Common/Constants.h"
#include "TabBar.h"

TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
    lineEdit_.setParent(this);
    lineEdit_.setValidator(new QRegExpValidator(QRegExp(Constants::datasetNameRegExp_), this));
    lineEdit_.hide();

    connect(&lineEdit_, SIGNAL(editingFinished()), this, SLOT(editingOfnameFinished()));

    lineEdit_.installEventFilter(this);
}

TabBar::~TabBar()
{

}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    int index = currentIndex();
    QRect tabRectangle = tabRect(index);
    lineEdit_.move(tabRectangle.x(), tabRectangle.y());
    lineEdit_.resize(tabRectangle.size());

    QTabWidget* tabWidget = static_cast<QTabWidget*>(parentWidget());
    QMainWindow* mainWindow = static_cast<QMainWindow*>(tabWidget->currentWidget());
    lineEdit_.setText(mainWindow->windowTitle());
    lineEdit_.show();
    lineEdit_.setFocus();

    QTabBar::mouseDoubleClickEvent(event);
}

void TabBar::editingOfnameFinished()
{
    if ( true == lineEdit_.isVisible() )
    {
        //Currently in eksport window is used name set as window title.
        int index = currentIndex();
        lineEdit_.hide();
        QString currentTabText = tabText(index);

        QTabWidget* tabWidget = static_cast<QTabWidget*>(parentWidget());
        QMainWindow* mainWindow = static_cast<QMainWindow*>(tabWidget->currentWidget());
        QString suffix = currentTabText.remove(0, mainWindow->windowTitle().length());
        setTabText(index, lineEdit_.text() + suffix);
        mainWindow->setWindowTitle(lineEdit_.text());
    }
}

bool TabBar::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if ( nullptr != keyEvent && Qt::Key_Escape == keyEvent->key() )
        {
            lineEdit_.hide();
            return true;
        }
    }

    //Standard event processing
    return QObject::eventFilter(obj, event);
}
