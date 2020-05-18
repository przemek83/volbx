#pragma once

#include <QCheckBox>

#include "LogType.h"

class CheckBox : public QCheckBox
{
    Q_OBJECT
public:
    CheckBox(LogTypes type, const QString& content, QWidget* parent = nullptr);

    ~CheckBox() override = default;

    LogTypes logType();

private:
    LogTypes type_;
};
