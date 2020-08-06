#include "Logger.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTime>
#include <QVBoxLayout>

#include "LoggerCheckBox.h"

Logger::Logger([[maybe_unused]] QObject* parent)
    : logNames_{{LogTypes::DB, "DATA_BASE"},
                {LogTypes::CONFIG, "CONFIG"},
                {LogTypes::MODEL, "DATA_MODEL"},
                {LogTypes::CALC, "CALCULATIONS"},
                {LogTypes::NETWORK, "NETWORK"},
                {LogTypes::LOGIN, "LOGIN"},
                {LogTypes::APP, "APPLICATION"},
                {LogTypes::IMPORT_EXPORT, "IMPORT_EXPORT"}}
{
    display_.setWindowTitle(QStringLiteral("Logs"));

    auto verticalLayout{new QVBoxLayout()};
    auto horizontalLayout{new QHBoxLayout()};

    textEdit_ = new QTextEdit(&display_);
    textEdit_->setLineWrapMode(QTextEdit::NoWrap);
    textEdit_->setReadOnly(true);

    verticalLayout->addStretch();

    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addWidget(textEdit_);

    display_.setLayout(horizontalLayout);

    // Default config, set all active.
    for (int i = 0; i < static_cast<int>(LogTypes::END); ++i)
        activeLogs_[static_cast<LogTypes>(i)] = true;

    reloadCheckBoxes();

    const int defaultLoggerWindowWidth{600};
    const int defaultLoggerWindowHeight{400};
    display_.resize(defaultLoggerWindowWidth, defaultLoggerWindowHeight);
}

Logger* Logger::getInstance()
{
    static auto instance = new Logger();
    return instance;
}

void Logger::log(LogTypes type, const char* file, const char* function,
                 int line, const QString& msg)
{
    if (textEdit_ == nullptr)
        return;

    // TODO Use __file__ and __line__
    if (!activeLogs_[type])
        return;

    const QString timeStyleBegin(
        QStringLiteral("<b><font size=\"3\" color=\"blue\">"));
    const QString fileStyleBegin(
        QStringLiteral("<b><font size=\"3\" color=\"black\">"));
    const QString functionStyleBegin(
        QStringLiteral("<b><font size=\"3\" color=\"red\">"));
    const QString lineStyleBegin(
        QStringLiteral("<b><font size=\"3\" color=\"green\">"));
    const QString styleEnd(QStringLiteral("</b></font>"));

    QString time;
    time.append(QTime::currentTime().toString(QStringLiteral("hh:mm:ss")));
    textEdit_->insertHtml(
        timeStyleBegin + time + styleEnd + QStringLiteral(" (") +
        logNames_[type] + QStringLiteral(")") + QStringLiteral(" - ") +
        functionStyleBegin + QLatin1String(function) + styleEnd +
        QStringLiteral(", ") + fileStyleBegin + QLatin1String(file) + styleEnd +
        QStringLiteral(" (") + lineStyleBegin + QString::number(line) +
        styleEnd + QStringLiteral(")") + QStringLiteral(":<br>"));

    textEdit_->insertPlainText(msg + QStringLiteral("\n\n"));

    QTextCursor c{textEdit_->textCursor()};
    c.movePosition(QTextCursor::End);
    textEdit_->setTextCursor(c);
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
