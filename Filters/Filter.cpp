#include "Filter.h"

#include <QDebug>

#include "Common/Constants.h"
#include "Shared/Application.h"

Filter::Filter(const QString& title, QWidget* parent) :
    QGroupBox(title, parent)
{
    setCheckable(true);
    setStyleSheet(Application::getGroupBoxStyle());
}
