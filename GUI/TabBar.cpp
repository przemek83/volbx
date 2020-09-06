#include "TabBar.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QRegExpValidator>
#include <QTabWidget>

#include <Common/DatasetUtilities.h>

TabBar::TabBar(QWidget* parent) : QTabBar(parent), lineEdit_(this)
{
    setupLineEdit();
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    const QRect tabRectangle{tabRect(currentIndex())};
    lineEdit_.move(tabRectangle.x(), tabRectangle.y());
    lineEdit_.resize(tabRectangle.size());

    auto tabWidget{dynamic_cast<QTabWidget*>(parentWidget())};
    auto mainWindow{dynamic_cast<QMainWindow*>(tabWidget->currentWidget())};
    lineEdit_.setText(mainWindow->windowTitle());
    lineEdit_.show();
    lineEdit_.setFocus();

    QTabBar::mouseDoubleClickEvent(event);
}

void TabBar::editingOfnameFinished()
{
    if (!lineEdit_.isVisible())
        return;

    const int index{currentIndex()};
    lineEdit_.hide();
    QString currentTabText{tabText(index)};

    const auto tabWidget{dynamic_cast<QTabWidget*>(parentWidget())};
    auto mainWindow{dynamic_cast<QMainWindow*>(tabWidget->currentWidget())};
    const QString suffix{
        currentTabText.remove(0, mainWindow->windowTitle().length())};
    setTabText(index, lineEdit_.text() + suffix);
    mainWindow->setWindowTitle(lineEdit_.text());
}

bool TabBar::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto keyEvent{dynamic_cast<QKeyEvent*>(event)};
        if (keyEvent != nullptr && keyEvent->key() == Qt::Key_Escape)
        {
            lineEdit_.hide();
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

void TabBar::setupLineEdit()
{
    lineEdit_.setValidator(new QRegExpValidator(
        QRegExp(DatasetUtilities::getDatasetNameRegExp()), this));
    lineEdit_.hide();

    connect(&lineEdit_, &QLineEdit::editingFinished, this,
            &TabBar::editingOfnameFinished);

    lineEdit_.installEventFilter(this);
}
