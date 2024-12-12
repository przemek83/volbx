#pragma once

#include <QLatin1String>
#include <QMap>
#include <QObject>
#include <QWidget>

#include "LogType.h"

#define LOG(type, msg) \
    Logger::getInstance().log(type, __FILE__, __FUNCTION__, __LINE__, msg)

class QTextEdit;
class QWidget;
class QHBoxLayout;
class QVBoxLayout;

/**
 * @brief Visual Logger class.
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    Logger& operator=(const Logger& other) = delete;
    Logger(const Logger& other) = delete;

    Logger& operator=(Logger&& other) = delete;
    Logger(Logger&& other) = delete;

    static Logger& getInstance();

    void log(LogTypes type, const char* file, const char* function, int line,
             const QString& msg);

    /**
     * @brief Toggle visibility of logging window.
     */
    void toggleVisibility();

private:
    explicit Logger(QObject* parent = nullptr);
    ~Logger() override = default;

    QTextEdit* createLogsTextEdit();

    QHBoxLayout* createLayout();

    void createCheckBoxes();

    static void moveCursorToTheEnd(QTextEdit* logTextEdit);

    QMap<LogTypes, bool> activeLogs_;

    /// Widget to display logs (text edit on it).
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

private Q_SLOTS:
    void changeActiveLogs(bool state);
};
