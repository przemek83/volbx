#include "Logger.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTime>
#include <QVBoxLayout>

#include "LoggerCheckBox.h"

Logger::Logger(QObject* parent) : QObject(parent)
{
    display_.setWindowTitle(QStringLiteral("Logs"));
    display_.setLayout(createLayout());

    // Activate all logs as default.
    for (int i = 0; i < static_cast<int>(LogTypes::END); ++i)
        activeLogs_[static_cast<LogTypes>(i)] = true;

    reloadCheckBoxes();

    const int defaultLoggerWindowWidth{600};
    const int defaultLoggerWindowHeight{400};
    display_.resize(defaultLoggerWindowWidth, defaultLoggerWindowHeight);
}

QTextEdit* Logger::createLogsTextEdit()
{
    auto textEdit{new QTextEdit(&display_)};
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    textEdit->setReadOnly(true);
    return textEdit;
}

QHBoxLayout* Logger::createLayout()
{
    auto verticalLayout{new QVBoxLayout()};
    verticalLayout->addStretch();
    auto horizontalLayout{new QHBoxLayout()};
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addWidget(createLogsTextEdit());
    return horizontalLayout;
}

Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::log(LogTypes type, const char* file, const char* function,
                 int line, const QString& msg)
{
    auto* logTextEdit{display_.findChild<QTextEdit*>()};
    if (logTextEdit == nullptr)
        return;

    if (!activeLogs_[type])
        return;

    QString time;
    time.append(QTime::currentTime().toString(QStringLiteral("hh:mm:ss")));
    logTextEdit->insertHtml(
        timeStyleBegin_ + time + styleEnd_ + QStringLiteral(" (") +
        logNames_[type] + QStringLiteral(")") + QStringLiteral(" - ") +
        functionStyleBegin_ + QLatin1String(function) + styleEnd_ +
        QStringLiteral(", ") + fileStyleBegin_ + QLatin1String(file) +
        styleEnd_ + QStringLiteral(" (") + lineStyleBegin_ +
        QString::number(line) + styleEnd_ + QStringLiteral(")") +
        QStringLiteral(":<br>"));

    logTextEdit->insertPlainText(msg + QStringLiteral("\n\n"));

    QTextCursor c{logTextEdit->textCursor()};
    c.movePosition(QTextCursor::End);
    logTextEdit->setTextCursor(c);
}

void Logger::reloadCheckBoxes()
{
    auto verticalLayout{display_.findChild<QVBoxLayout*>()};
    if (verticalLayout == nullptr)
        return;

    // Delete all.
    QList<QCheckBox*> checkBoxes{verticalLayout->findChildren<QCheckBox*>()};
    for (QCheckBox* checkBox : checkBoxes)
    {
        verticalLayout->removeWidget(checkBox);
        delete checkBox;
    }

    // Create new.
    QMapIterator<LogTypes, bool> i(activeLogs_);
    while (i.hasNext())
    {
        i.next();

        LoggerCheckBox* check{
            new LoggerCheckBox(i.key(), logNames_[i.key()], &display_)};
        check->setChecked(i.value());
        connect(check, &LoggerCheckBox::toggled, this,
                &Logger::changeActiveLogs);
        verticalLayout->insertWidget(static_cast<int>(i.key()), check);
    }
}
void Logger::changeActiveLogs(bool state)
{
    auto checkBox{qobject_cast<LoggerCheckBox*>(sender())};
    activeLogs_[checkBox->logType()] = state;
    QString msg(logNames_[checkBox->logType()]);
    msg.append(QStringLiteral(" is ") + (state ? QStringLiteral("enabled")
                                               : QStringLiteral("disabled")));
    LOG(LogTypes::APP, msg);
}

void Logger::switchVisibility() { display_.setVisible(!display_.isVisible()); }
