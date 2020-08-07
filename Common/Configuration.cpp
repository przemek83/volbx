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

bool Configuration::isUpdatePolicyPicked() const
{
    return (configValid_ && updatePolicy_ != UpdatePolicy::NOT_DECIDED);
}

bool Configuration::needToCheckForUpdates() const
{
    return (updatePolicy_ == UpdatePolicy::ALWAYS_CHECK);
}

bool Configuration::load()
{
    configValid_ = false;

    // Default style.
    if (styleName_.isEmpty())
        styleName_ = QStringLiteral("Fusion");

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
        updatePolicy_ = static_cast<UpdatePolicy>(
            updateElement.attribute(XML_NAME_VALUE).toInt());

    list = configXML.elementsByTagName(XML_NAME_STYLE);
    QDomElement styleElement = list.at(0).toElement();
    if (!styleElement.isNull())
        styleName_ = styleElement.attribute(XML_NAME_VALUE);

    list = configXML.elementsByTagName(XML_NAME_IMPORTPATH);
    QDomElement importPathElement = list.at(0).toElement();
    if (!importPathElement.isNull())
        importFilePath_ = importPathElement.attribute(XML_NAME_VALUE);

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
                         QString::number(static_cast<int>(updatePolicy_)));
    root.appendChild(updates);

    QDomElement style = doc.createElement(XML_NAME_STYLE);
    style.setAttribute(XML_NAME_VALUE, styleName_);
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

    if (file.write(xml.toStdString().c_str()) != -1)
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
    dump.append((updatePolicy_ == UpdatePolicy::NOT_DECIDED
                     ? QLatin1String("No")
                     : QLatin1String("Yes")));

    dump.append(QLatin1String("\n"));

    dump.append("Import file path = " + importFilePath_ + "\n");

    if (updatePolicy_ != UpdatePolicy::NOT_DECIDED)
    {
        dump.append(QLatin1String("AutoUpdate active = "));
        dump.append((updatePolicy_ == UpdatePolicy::ALWAYS_CHECK
                         ? QLatin1String("Yes")
                         : QLatin1String("No")));
        dump.append(QLatin1String("\n"));
    }

    dump.append("Style: " + styleName_);

    return dump;
}

void Configuration::setUpdatesCheckingOption(bool alwaysCheck)
{
    updatePolicy_ =
        (alwaysCheck ? UpdatePolicy::ALWAYS_CHECK : UpdatePolicy::NEVER_CHECK);
}

QString Configuration::getStyleName() const { return styleName_; }

void Configuration::setStyleName(const QString& style) { styleName_ = style; }

bool Configuration::configValid() const { return configValid_; }

QString Configuration::getImportFilePath() const
{
    if (!importFilePath_.isEmpty() && QFile::exists(importFilePath_))
        return importFilePath_;
    return QDir::homePath();
}

void Configuration::setImportFilePath(const QString& path)
{
    importFilePath_ = path;
}
