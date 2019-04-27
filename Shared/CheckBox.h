#pragma once

#include <QCheckBox>

#include "LogType.h"

class CheckBox : public QCheckBox
{
    Q_OBJECT
public:
    CheckBox(LogTypes type, const QString& content, QWidget* parent = nullptr);

    ~CheckBox() override;

    CheckBox& operator=(const CheckBox& other) = delete;
    CheckBox(const CheckBox& other) = delete;

    CheckBox& operator=(CheckBox&& other) = delete;
    CheckBox(CheckBox&& other) = delete;

    LogTypes logType();

private:
    LogTypes type_;
};
