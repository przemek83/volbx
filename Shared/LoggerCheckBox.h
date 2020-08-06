#pragma once

#include <QCheckBox>

#include "LogType.h"

class LoggerCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    LoggerCheckBox(LogTypes type, const QString& content,
                   QWidget* parent = nullptr);

    ~LoggerCheckBox() override = default;

    LogTypes logType();

private:
    LogTypes type_;
};
