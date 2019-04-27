#include "CheckBox.h"

CheckBox::CheckBox(LogTypes type, const QString& content, QWidget* parent)
    : QCheckBox(content, parent), type_(type)
{

}

CheckBox::~CheckBox()
{

}

LogTypes CheckBox::logType()
{
    return type_;
}

