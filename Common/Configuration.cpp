#include <QString>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QApplication>

#include "Constants.h"
#include "Shared/Logger.h"

#include "Configuration.h"

const char* Configuration::xmlNames_[] =
{
    "CONFIG",
    "UPDATE",
    "VALUE",
    "STYLE",
    "IMPORTPATH"
};

Configuration::Configuration()
{
    load();
}

Configuration& Configuration::getInstance()
{
    static Configuration instance;
    return instance;
}

bool Configuration::needToShowUpdatePickerDialog() const
{
    return (configValid_ && UPDATES_CHOICE_NOT_PICKED == updateOption_);
}

bool Configuration::needToCheckForUpdates() const
{
    return (UPDATES_ALWAYS_CHECK == updateOption_);
}

bool Configuration::load()
{
    configValid_ = false;

    //Default style.
    if (style_.isEmpty())
    {
        style_ = QStringLiteral("Fusion");
    }

    QString filename(QApplication::applicationDirPath() +
                     "/" +
                     Constants::configurationFile_);

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
    {
        LOG(LOG_CONFIG,
            "Config file " + filename + " can not be opened. Default config used.");

        return false;
    }

    LOG(LOG_CONFIG, "Found config file " + filename + ".");

    QTextStream stream(&file);
    QString initial(stream.readAll());
    QDomDocument configXML(__FUNCTION__);

    //If could not parse config.
    if (!configXML.setContent(initial))
    {
        LOG(LOG_CONFIG, "Config file " + filename +
            " is damaged and will be deleted. Default config used.");

        file.close();

        return false;
    }

    file.close();

    LOG(LOG_CONFIG, "Loaded config file:\n" + configXML.toString());

    QDomNodeList list = configXML.elementsByTagName(xmlNames_[XML_NAME_UPDATE]);
    QDomElement updateElement = list.at(0).toElement();
    if (!updateElement.isNull())
    {
        updateOption_ = static_cast<UpdateOption>(
                            updateElement.attribute(xmlNames_[XML_NAME_VALUE]).toInt());
    }

    list = configXML.elementsByTagName(xmlNames_[XML_NAME_STYLE]);
    QDomElement styleElement = list.at(0).toElement();
    if (!styleElement.isNull())
    {
        style_ = styleElement.attribute(xmlNames_[XML_NAME_VALUE]);
    }

    list = configXML.elementsByTagName(xmlNames_[XML_NAME_IMPORTPATH]);
    QDomElement importPathElement = list.at(0).toElement();
    if (!importPathElement.isNull())
    {
        importFilePath_ =
            importPathElement.attribute(xmlNames_[XML_NAME_VALUE]);
    }

    LOG(LOG_CONFIG, configDump());

    configValid_ = true;

    return true;
}

bool Configuration::save()
{
    QDomDocument doc(xmlNames_[XML_NAME_CONFIG]);
    QDomElement root = doc.createElement(xmlNames_[XML_NAME_CONFIG]);
    doc.appendChild(root);

    QDomElement updates = doc.createElement(xmlNames_[XML_NAME_UPDATE]);
    updates.setAttribute(xmlNames_[XML_NAME_VALUE],
                         QString::number(static_cast<int>(updateOption_)));
    root.appendChild(updates);

    QDomElement style = doc.createElement(xmlNames_[XML_NAME_STYLE]);
    style.setAttribute(xmlNames_[XML_NAME_VALUE], style_);
    root.appendChild(style);

    QDomElement importPath = doc.createElement(xmlNames_[XML_NAME_IMPORTPATH]);
    importPath.setAttribute(xmlNames_[XML_NAME_VALUE], importFilePath_);
    root.appendChild(importPath);

    QString xml = doc.toString();

    LOG(LOG_CONFIG, "Config to save:\n" + xml);

    QString filename(QApplication::applicationDirPath() + "/" +
                     QString(Constants::configurationFile_));
    QFile::remove(filename);
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        LOG(LOG_CONFIG, "Config file " + QString(Constants::configurationFile_) +
            " can not be opened for writing. Config not saved.");
        return false;
    }

    if (-1 != file.write(xml.toStdString().c_str()))
    {
        LOG(LOG_CONFIG, "Config saved.");
        file.close();
        return true;
    }

    LOG(LOG_CONFIG, "Error during config file save.");
    file.close();
    return false;
}

QString Configuration::configDump() const
{
    QString dump;

    dump.append("Configuration(" + QString(Constants::configurationFile_) + "):\n");

    dump.append(QLatin1String("Updates choice picked = "));
    dump.append((updateOption_ == UPDATES_CHOICE_NOT_PICKED ? QLatin1String("No") : QLatin1String("Yes")));

    dump.append(QLatin1String("\n"));

    dump.append("Import file path = " + importFilePath_ + "\n");

    if (updateOption_ != UPDATES_CHOICE_NOT_PICKED)
    {
        dump.append(QLatin1String("AutoUpdate active = "));
        dump.append((updateOption_ == UPDATES_ALWAYS_CHECK ? QLatin1String("Yes") : QLatin1String("No")));
        dump.append(QLatin1String("\n"));
    }

    dump.append("Style: " + style_);

    return dump;
}

void Configuration::setUpdatesCheckingOption(bool alwaysCheck)
{
    if (alwaysCheck)
    {
        updateOption_ = UPDATES_ALWAYS_CHECK;
    }
    else
    {
        updateOption_ = UPDATES_NEVER_CHECK;
    }
}

QString Configuration::getStyle() const
{
    return style_;
}

void Configuration::setStyle(const QString& style)
{
    style_ = style;
}

bool Configuration::configWasValid() const
{
    return configValid_;
}

QString Configuration::getImportFilePath() const
{
    if (!importFilePath_.isEmpty() && QFile::exists(importFilePath_))
    {
        return importFilePath_;
    }

    return QDir::homePath();
}

void Configuration::setImportFilePath(const QString& path)
{
    importFilePath_ = path;
}
