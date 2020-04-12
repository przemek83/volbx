#include "CheckBox.h"

CheckBox::CheckBox(LogTypes type, const QString& content, QWidget* parent)
    : QCheckBox(content, parent), type_(type)
{
}

LogTypes CheckBox::logType() { return type_; }
