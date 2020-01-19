#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QMap>
#include <QObject>
#include <QCheckBox>
#include <QLatin1String>

#include "Debugging.h"
#include "LogType.h"
#include "CheckBox.h"

//If debug is not activated use empty logger. For debugging on use logging window.
#ifdef DEBUGGING
#define LOG(type, msg) Logger::getInstance()->log(type, __FILE__, __FUNCTION__, __LINE__, msg)
#else
#define LOG(...)
#endif

class QTextEdit;
class QWidget;

/**
 * @brief GUI Logger class.
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    Logger& operator=(const Logger& other) = delete;
    Logger(const Logger& other) = delete;

    Logger& operator=(Logger&& other) = delete;
    Logger(Logger&& other) = delete;

    static Logger* getInstance();

    void log(LogTypes type,
             const char* file,
             const char* function,
             int line,
             const QString& msg);

    void switchVisibility();

private:
    explicit Logger(QObject* parent = nullptr);
    ~Logger() override = default;

    QMap<LogTypes, bool> activeLogs_;

    void reloadCheckBoxes();

    ///Widget to display logs (text edit on it).
    QWidget display_;

    QTextEdit* textEdit_ {nullptr};

    const static char* LogTypeNames_[];

private Q_SLOTS:
    void changeActiveLogs(bool state);
};

#endif // LOGWINDOW_H
