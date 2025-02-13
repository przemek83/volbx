#pragma once

#include <QMap>
#include <QObject>
#include <QTextEdit>
#include <QWidget>

#include "LogType.h"

#define LOG(type, msg)                                                         \
    Logger::getInstance().info((type), QStringLiteral(__FILE__), __FUNCTION__, \
                               __LINE__, (msg))

class QTextEdit;
class QWidget;
class QHBoxLayout;
class QVBoxLayout;

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger& getInstance();

    void info(LogTypes type, const QString& file, const char* function,
              int line, const QString& msg);

    void toggleVisibility();

private:
    Logger();

    QHBoxLayout* createLayout();

    void createCheckBoxes();

    static void moveCursorToTheEnd(QTextEdit* logTextEdit);

    QMap<LogTypes, bool> activeLogs_;

    QWidget display_;

    const QMap<LogTypes, QString> logNames_{
        {LogTypes::DB, "DATA_BASE"},
        {LogTypes::CONFIG, "CONFIG"},
        {LogTypes::MODEL, "DATA_MODEL"},
        {LogTypes::CALC, "CALCULATIONS"},
        {LogTypes::NETWORK, "NETWORK"},
        {LogTypes::LOGIN, "LOGIN"},
        {LogTypes::APP, "APPLICATION"},
        {LogTypes::IMPORT_EXPORT, "IMPORT_EXPORT"}};

    const QString timeStyleBegin_{
        QStringLiteral("<b><font size=\"3\" color=\"blue\">")};
    const QString fileStyleBegin_{
        QStringLiteral("<b><font size=\"3\" color=\"black\">")};
    const QString functionStyleBegin_{
        QStringLiteral("<b><font size=\"3\" color=\"red\">")};
    const QString lineStyleBegin_{
        QStringLiteral("<b><font size=\"3\" color=\"green\">")};
    const QString styleEnd_{QStringLiteral("</b></font>")};

    QTextEdit textEdit_;

private slots:
    void changeActiveLogs(bool state);
};
