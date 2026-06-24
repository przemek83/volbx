#include "TabBar.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QRegularExpressionValidator>
#include <QTabWidget>

TabBar::TabBar() : nameEdit_(this) { setupLineEdit(); }

void TabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    const QRect tabRectangle{tabRect(currentIndex())};
    nameEdit_.move(tabRectangle.topLeft());
    nameEdit_.resize(tabRectangle.size());

    nameEdit_.setText(getCurrentTabName());
    nameEdit_.show();
    nameEdit_.setFocus();

    QTabBar::mouseDoubleClickEvent(event);
}

void TabBar::editingNameFinished()
{
    if (!nameEdit_.isVisible())
        return;

    const int index{currentIndex()};
    nameEdit_.hide();
    QString currentTabText{tabText(index)};

    const QString suffix{
        currentTabText.remove(0, getCurrentTabName().length())};
    setTabText(index, nameEdit_.text() + suffix);
    setCurrentTabName(nameEdit_.text());
}

bool TabBar::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        const auto* keyEvent{dynamic_cast<QKeyEvent*>(event)};
        if ((keyEvent != nullptr) && (keyEvent->key() == Qt::Key_Escape))
        {
            nameEdit_.hide();
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void TabBar::setupLineEdit()
{
    nameEdit_.setValidator(&validator_);
    nameEdit_.hide();
    connect(&nameEdit_, &QLineEdit::editingFinished, this,
            &TabBar::editingNameFinished);
    nameEdit_.installEventFilter(this);
}

QMainWindow* TabBar::getCurrentTabWidget() const
{
    const auto* tabWidget{dynamic_cast<QTabWidget*>(parentWidget())};
    if (tabWidget == nullptr)
        return nullptr;
    auto* mainWindow{dynamic_cast<QMainWindow*>(tabWidget->currentWidget())};
    return mainWindow;
}

QString TabBar::getCurrentTabName() const
{
    const QMainWindow* mainWindow{getCurrentTabWidget()};
    if (mainWindow == nullptr)
        return {};
    return mainWindow->windowTitle();
}

void TabBar::setCurrentTabName(const QString& newName)
{
    QMainWindow* mainWindow{getCurrentTabWidget()};
    if (mainWindow == nullptr)
        return;
    mainWindow->setWindowTitle(newName);
}
