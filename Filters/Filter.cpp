#include "Filter.h"

#include <QDebug>

#include "Common/Constants.h"
#include "Shared/Application.h"

Filter::Filter(const QString& title, int column, QWidget* parent) :
    QGroupBox(title, parent), column_(column)
{
    setCheckable(true);
    setStyleSheet(Application::getGroupBoxStyle());
}
