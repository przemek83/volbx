#pragma once

#include <QCheckBox>

#include "LogType.h"

/// Check box class used for enabling or disabling logs categories.
class LoggerCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    LoggerCheckBox(LogTypes type, const QString& content, QWidget* parent);

    LogTypes logType() const;

private:
    LogTypes type_;
};
