#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QLatin1String>
#include <QMap>
#include <QObject>
#include <QWidget>

#include "LogType.h"

#define LOG(type, msg) \
    Logger::getInstance().log(type, __FILE__, __FUNCTION__, __LINE__, msg)

class QTextEdit;
class QWidget;

/**
 * @brief GUI Logger class.
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger& getInstance();

    void log(LogTypes type, const char* file, const char* function, int line,
             const QString& msg);

    void switchVisibility();

private:
    explicit Logger(QObject* parent = nullptr);
    ~Logger() override = default;

    void reloadCheckBoxes();

    QMap<LogTypes, bool> activeLogs_;

    /// Widget to display logs (text edit on it).
    QWidget display_;

    QTextEdit* textEdit_{nullptr};

    const QMap<LogTypes, QString> logNames_;

private Q_SLOTS:
    void changeActiveLogs(bool state);
};

#endif  // LOGWINDOW_H
