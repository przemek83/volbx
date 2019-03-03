#include <QDebug>
#include <QTime>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "Logger.h"

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

Logger::Logger()
{
    display_ = new QWidget();
    display_->setWindowTitle(QLatin1String("Logs"));

    QVBoxLayout* verticalLayout = new QVBoxLayout();
    QHBoxLayout* horizontalLayout = new QHBoxLayout();

    textEdit_ = new QTextEdit(display_);
    textEdit_->setLineWrapMode(QTextEdit::NoWrap);
    textEdit_->setReadOnly(true);

    verticalLayout->addStretch();

    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addWidget(textEdit_);

    display_->setLayout(horizontalLayout);

    //Default config, set all active.
    activeLogs_ = new QMap<LogTypes, bool>();
    for (int i = 0; i < static_cast<int>(LOG_END); ++i)
    {
        (*activeLogs_)[static_cast<LogTypes>(i)] = true;
    }

    reloadCheckBoxes();

    display_->resize(600, 400);
    display_->show();
}

Logger::~Logger()
{
    delete display_;
    delete activeLogs_;
}

Logger* Logger::getInstance()
{
    static Logger* instance = new Logger();
    return instance;
}

void Logger::log(LogTypes type,
                 const char* file,
                 const char* function,
                 int line,
                 QString msg)
{
    Q_ASSERT(nullptr != display_ && nullptr != textEdit_);

    if (nullptr == display_ || nullptr == textEdit_)
    {
        return;
    }

    //TODO Use __file__ and __line__
    if (!(*activeLogs_)[type])
    {
        return;
    }

    static QString timeStyleBegin(QLatin1String("<b><font size=\"3\" color=\"blue\">"));
    static QString fileStyleBegin(QLatin1String("<b><font size=\"3\" color=\"black\">"));
    static QString functionStyleBegin(QLatin1String("<b><font size=\"3\" color=\"red\">"));
    static QString lineStyleBegin(QLatin1String("<b><font size=\"3\" color=\"green\">"));
    static QString styleEnd(QLatin1String("</b></font>"));

    QString time;
    time.append(QTime::currentTime().toString(QLatin1String("hh:mm:ss")));
    textEdit_->insertHtml(timeStyleBegin + time + styleEnd +
                          QLatin1String(" (") + QLatin1String(LogTypeNames_[static_cast<int>(type)]) + QLatin1String(")") +
                          QLatin1String(" - ") +
                          functionStyleBegin + QLatin1String(function) + styleEnd +
                          QLatin1String(", ") +
                          fileStyleBegin + QLatin1String(file) + styleEnd +
                          QLatin1String(" (") + lineStyleBegin + QString::number(line) + styleEnd + QLatin1String(")") +
                          QLatin1String(":<br>"));

    textEdit_->insertPlainText(msg + QLatin1String("\n\n"));

    QTextCursor c =  textEdit_->textCursor();
    c.movePosition(QTextCursor::End);
    textEdit_->setTextCursor(c);
}

void Logger::reloadCheckBoxes()
{
    QVBoxLayout* verticalLayout = display_->findChild<QVBoxLayout*>();

    Q_ASSERT(verticalLayout != nullptr);

    if (verticalLayout == nullptr)
    {
        return;
    }

    //Delete all.
    QList<QCheckBox*> checkBoxy = verticalLayout->findChildren<QCheckBox*>();
    for (QCheckBox* checkBox : checkBoxy)
    {
        verticalLayout->removeWidget(checkBox);
        delete checkBox;
    }

    //Create new.
    QMapIterator<LogTypes, bool> i(*activeLogs_);
    while (i.hasNext())
    {
        i.next();

        CheckBox* check = new CheckBox(i.key(), QLatin1String(LogTypeNames_[static_cast<int>(i.key())]), display_);
        check->setChecked(i.value());
        connect(check, SIGNAL(toggled(bool)), this, SLOT(changeActiveLogs(bool)));
        verticalLayout->insertWidget(static_cast<int>(i.key()), check);
    }
}

Logger::CheckBox::CheckBox(LogTypes type, QString content, QWidget* parent)
    : QCheckBox(content, parent), type_(type)
{

}

Logger::CheckBox::~CheckBox()
{

}

LogTypes Logger::CheckBox::logType()
{
    return type_;
}

void Logger::changeActiveLogs(bool state)
{
    auto checkBox = dynamic_cast<CheckBox*>(sender());
    (*activeLogs_)[checkBox->logType()] = state;
    QString msg(LogTypeNames_[checkBox->logType()]);
    msg.append(QLatin1String(" is ") + (state ? QLatin1String("active") : QLatin1String("disabled")));
    LOG(LOG_APP, msg);
}


void Logger::switchVisibility()
{
    if (nullptr != display_)
    {
        display_->setVisible(!display_->isVisible());
    }
}
