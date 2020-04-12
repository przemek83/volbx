#include "Configuration.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QtXml/QDomDocument>

#include "Constants.h"
#include "Shared/Logger.h"

Configuration::Configuration() { load(); }

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

    // Default style.
    if (style_.isEmpty())
    {
        style_ = QStringLiteral("Fusion");
    }

    QString filename(QApplication::applicationDirPath() + "/" +
                     Constants::getConfigurationFileName());

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
    {
        LOG(LogTypes::CONFIG, "Config file " + filename +
                                  " can not be opened. Default config used.");

        return false;
    }

    LOG(LogTypes::CONFIG, "Found config file " + filename + ".");

    QTextStream stream(&file);
    QString initial(stream.readAll());
    QDomDocument configXML(filename);

    // If could not parse config.
    if (!configXML.setContent(initial))
    {
        LOG(LogTypes::CONFIG,
            "Config file " + filename +
                " is damaged and will be deleted. Default config used.");

        file.close();

        return false;
    }

    file.close();

    LOG(LogTypes::CONFIG, "Loaded config file:\n" + configXML.toString());

    QDomNodeList list = configXML.elementsByTagName(XML_NAME_UPDATE);
    QDomElement updateElement = list.at(0).toElement();
    if (!updateElement.isNull())
    {
        updateOption_ = static_cast<UpdateOption>(
            updateElement.attribute(XML_NAME_VALUE).toInt());
    }

    list = configXML.elementsByTagName(XML_NAME_STYLE);
    QDomElement styleElement = list.at(0).toElement();
    if (!styleElement.isNull())
    {
        style_ = styleElement.attribute(XML_NAME_VALUE);
    }

    list = configXML.elementsByTagName(XML_NAME_IMPORTPATH);
    QDomElement importPathElement = list.at(0).toElement();
    if (!importPathElement.isNull())
    {
        importFilePath_ = importPathElement.attribute(XML_NAME_VALUE);
    }

    LOG(LogTypes::CONFIG, configDump());

    configValid_ = true;

    return true;
}

bool Configuration::save()
{
    QDomDocument doc(XML_NAME_CONFIG);
    QDomElement root = doc.createElement(XML_NAME_CONFIG);
    doc.appendChild(root);

    QDomElement updates = doc.createElement(XML_NAME_UPDATE);
    updates.setAttribute(XML_NAME_VALUE,
                         QString::number(static_cast<int>(updateOption_)));
    root.appendChild(updates);

    QDomElement style = doc.createElement(XML_NAME_STYLE);
    style.setAttribute(XML_NAME_VALUE, style_);
    root.appendChild(style);

    QDomElement importPath = doc.createElement(XML_NAME_IMPORTPATH);
    importPath.setAttribute(XML_NAME_VALUE, importFilePath_);
    root.appendChild(importPath);

    QString xml = doc.toString();

    LOG(LogTypes::CONFIG, "Config to save:\n" + xml);

    QString filename(QApplication::applicationDirPath() + "/" +
                     Constants::getConfigurationFileName());
    QFile::remove(filename);
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        LOG(LogTypes::CONFIG,
            "Config file " + Constants::getConfigurationFileName() +
                " can not be opened for writing. Config not saved.");
        return false;
    }

    if (-1 != file.write(xml.toStdString().c_str()))
    {
        LOG(LogTypes::CONFIG, "Config saved.");
        file.close();
        return true;
    }

    LOG(LogTypes::CONFIG, "Error during config file save.");
    file.close();
    return false;
}

QString Configuration::configDump() const
{
    QString dump;

    dump.append("Configuration(" + Constants::getConfigurationFileName() +
                "):\n");

    dump.append(QLatin1String("Updates choice picked = "));
    dump.append((updateOption_ == UPDATES_CHOICE_NOT_PICKED
                     ? QLatin1String("No")
                     : QLatin1String("Yes")));

    dump.append(QLatin1String("\n"));

    dump.append("Import file path = " + importFilePath_ + "\n");

    if (updateOption_ != UPDATES_CHOICE_NOT_PICKED)
    {
        dump.append(QLatin1String("AutoUpdate active = "));
        dump.append((updateOption_ == UPDATES_ALWAYS_CHECK
                         ? QLatin1String("Yes")
                         : QLatin1String("No")));
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

QString Configuration::getStyle() const { return style_; }

void Configuration::setStyle(const QString& style) { style_ = style; }

bool Configuration::configWasValid() const { return configValid_; }

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
