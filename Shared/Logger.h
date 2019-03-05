#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <Qmap>
#include <QObject>
#include <QCheckBox>
#include <QLatin1String>

#include "Debugging.h"

//If debug is not activated use empty logger. For debugging on use logging window.
#ifdef DEBUGGING
#define LOG(type, msg) Logger::getInstance()->log(type, __FILE__, __FUNCTION__, __LINE__, msg)
#else
#define LOG(...)
#endif

class QTextEdit;
class QWidget;

/**
 * @brief types of logs.
 */
enum LogTypes
{
    LOG_DB = 0,
    LOG_CONFIG,
    LOG_MODEL,
    LOG_CALC,
    LOG_NETWORK,
    LOG_LOGIN,
    LOG_APP,
    LOG_IMPORT_EXPORT,
    LOG_END
};

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
             QString msg);

    void switchVisibility();

private:
    Logger();
    ~Logger() override;

    QMap<LogTypes, bool>* activeLogs_;

    class CheckBox : public QCheckBox
    {
    public:
        CheckBox(LogTypes type, QString content, QWidget* parent = nullptr);

        ~CheckBox() override;

        CheckBox& operator=(const CheckBox& other) = delete;
        CheckBox(const CheckBox& other) = delete;

        CheckBox& operator=(CheckBox&& other) = delete;
        CheckBox(CheckBox&& other) = delete;

        LogTypes logType();

    private:
        LogTypes type_;
    };

    void reloadCheckBoxes();

    ///Widget to display logs (text edit on it).
    QWidget* display_ {nullptr};

    QTextEdit* textEdit_ {nullptr};

    const static char* LogTypeNames_[];

private Q_SLOTS:
    void changeActiveLogs(bool state);
};

#endif // LOGWINDOW_H
