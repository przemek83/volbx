#include "LoggerCheckBox.h"

LoggerCheckBox::LoggerCheckBox(LogTypes type, const QString& content,
                               QWidget* parent)
    : QCheckBox(content, parent), type_(type)
{
}

LogTypes LoggerCheckBox::logType() { return type_; }
