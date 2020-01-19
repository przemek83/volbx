#include "Logger.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTime>
#include <QVBoxLayout>

const char* Logger::LogTypeNames_[] =
{
    "DATA_BASE",
    "CONFIG",
    "DATA_MODEL",
    "CALCULATIONS",
    "NETWORK",
    "LOGIN",
    "APPLICATION",
    "IMPORT_EXPORT"
};

Logger::Logger([[maybe_unused]] QObject* parent)
{
    display_.setWindowTitle(QStringLiteral("Logs"));

    auto verticalLayout = new QVBoxLayout();
    auto horizontalLayout = new QHBoxLayout();

    textEdit_ = new QTextEdit(&display_);
    textEdit_->setLineWrapMode(QTextEdit::NoWrap);
    textEdit_->setReadOnly(true);

    verticalLayout->addStretch();

    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addWidget(textEdit_);

    display_.setLayout(horizontalLayout);

    //Default config, set all active.
    for (int i = 0; i < static_cast<int>(LogTypes::END); ++i)
    {
        activeLogs_[static_cast<LogTypes>(i)] = true;
    }

    reloadCheckBoxes();

    display_.resize(600, 400);
    display_.show();
}

Logger* Logger::getInstance()
{
    static auto instance = new Logger();
    return instance;
}

void Logger::log(LogTypes type,
                 const char* file,
                 const char* function,
                 int line,
                 const QString& msg)
{
    Q_ASSERT(nullptr != textEdit_);

    if (nullptr == textEdit_)
    {
        return;
    }

    //TODO Use __file__ and __line__
    if (!activeLogs_[type])
    {
        return;
    }

    static QString timeStyleBegin(QStringLiteral("<b><font size=\"3\" color=\"blue\">"));
    static QString fileStyleBegin(QStringLiteral("<b><font size=\"3\" color=\"black\">"));
    static QString functionStyleBegin(QStringLiteral("<b><font size=\"3\" color=\"red\">"));
    static QString lineStyleBegin(QStringLiteral("<b><font size=\"3\" color=\"green\">"));
    static QString styleEnd(QStringLiteral("</b></font>"));

    QString time;
    time.append(QTime::currentTime().toString(QStringLiteral("hh:mm:ss")));
    textEdit_->insertHtml(timeStyleBegin + time + styleEnd +
                          QStringLiteral(" (") + QLatin1String(LogTypeNames_[static_cast<int>(type)]) + QStringLiteral(")") +
                          QStringLiteral(" - ") +
                          functionStyleBegin + QLatin1String(function) + styleEnd +
                          QStringLiteral(", ") +
                          fileStyleBegin + QLatin1String(file) + styleEnd +
                          QStringLiteral(" (") + lineStyleBegin + QString::number(line) + styleEnd + QStringLiteral(")") +
                          QStringLiteral(":<br>"));

    textEdit_->insertPlainText(msg + QStringLiteral("\n\n"));

    QTextCursor c =  textEdit_->textCursor();
    c.movePosition(QTextCursor::End);
    textEdit_->setTextCursor(c);
}

void Logger::reloadCheckBoxes()
{
    auto verticalLayout = display_.findChild<QVBoxLayout*>();

    Q_ASSERT(verticalLayout != nullptr);

    if (verticalLayout == nullptr)
    {
        return;
    }

    //Delete all.
    QList<QCheckBox*> checkBoxes = verticalLayout->findChildren<QCheckBox*>();
    for (QCheckBox* checkBox : checkBoxes)
    {
        verticalLayout->removeWidget(checkBox);
        delete checkBox;
    }

    //Create new.
    QMapIterator<LogTypes, bool> i(activeLogs_);
    while (i.hasNext())
    {
        i.next();

        CheckBox* check = new CheckBox(i.key(), QLatin1String(LogTypeNames_[static_cast<int>(i.key())]), &display_);
        check->setChecked(i.value());
        connect(check, SIGNAL(toggled(bool)), this, SLOT(changeActiveLogs(bool)));
        verticalLayout->insertWidget(static_cast<int>(i.key()), check);
    }
}
void Logger::changeActiveLogs(bool state)
{
    auto checkBox = qobject_cast<CheckBox*>(sender());
    activeLogs_[checkBox->logType()] = state;
    QString msg(LogTypeNames_[static_cast<int>(checkBox->logType())]);
    msg.append(QStringLiteral(" is ") + (state ? QStringLiteral("active") : QStringLiteral("disabled")));
    LOG(LogTypes::APP, msg);
}


void Logger::switchVisibility()
{
    display_.setVisible(!display_.isVisible());
}
