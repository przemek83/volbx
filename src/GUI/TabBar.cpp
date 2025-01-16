#include "TabBar.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QRegularExpressionValidator>
#include <QTabWidget>

#include <Common/DatasetUtilities.h>

TabBar::TabBar(QWidget* parent) : QTabBar(parent), nameEdit_(this)
{
    setupLineEdit();
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    const QRect tabRectangle{tabRect(currentIndex())};
    nameEdit_.move(tabRectangle.topLeft());
    nameEdit_.resize(tabRectangle.size());

    const QMainWindow* mainWindow{getCurrentTabWidget()};
    nameEdit_.setText(mainWindow->windowTitle());
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

    QMainWindow* mainWindow{getCurrentTabWidget()};
    const QString suffix{
        currentTabText.remove(0, mainWindow->windowTitle().length())};
    setTabText(index, nameEdit_.text() + suffix);
    mainWindow->setWindowTitle(nameEdit_.text());
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
    const auto* validator{new QRegularExpressionValidator(
        QRegularExpression(dataset_utilities::getDatasetNameRegExp()), this)};
    nameEdit_.setValidator(validator);
    nameEdit_.hide();
    connect(&nameEdit_, &QLineEdit::editingFinished, this,
            &TabBar::editingNameFinished);
    nameEdit_.installEventFilter(this);
}

QMainWindow* TabBar::getCurrentTabWidget() const
{
    const auto* tabWidget{dynamic_cast<QTabWidget*>(parentWidget())};
    auto* mainWindow{dynamic_cast<QMainWindow*>(tabWidget->currentWidget())};
    return mainWindow;
}
