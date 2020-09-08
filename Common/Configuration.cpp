#include "Configuration.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QtXml/QDomDocument>

#include "Shared/Logger.h"

#include "Constants.h"
#include "FileUtilities.h"

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

    if (styleName_.isEmpty())
        styleName_ = QStringLiteral("Fusion");

    QDomDocument configXML;
    if (!loadConfigXml(configXML))
        return false;

    parseConfigXml(configXML);

    LOG(LogTypes::CONFIG, configDump());

    configValid_ = true;

    return true;
}

bool Configuration::save()
{
    QString configXml{generateConfigXml()};

    LOG(LogTypes::CONFIG, "Config to save:\n" + configXml);

    return saveConfigXml(configXml);
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

bool Configuration::loadConfigXml(QDomDocument& configXml)
{
    const QString filename(QApplication::applicationDirPath() + "/" +
                           Constants::getConfigurationFileName());

    const auto [success, content] = FileUtilities::loadFile(filename);
    if (!success)
    {
        LOG(LogTypes::CONFIG, "Config file " + filename +
                                  " can not be opened. Default config used.");

        return false;
    }

    LOG(LogTypes::CONFIG, "Found config file " + filename + ".");

    if (!configXml.setContent(content))
    {
        LOG(LogTypes::CONFIG,
            "Config file " + filename +
                " is damaged and will be deleted. Default config used.");
        return false;
    }

    LOG(LogTypes::CONFIG, "Loaded config file:\n" + configXml.toString());

    return true;
}

void Configuration::parseConfigXml(QDomDocument& configXml)
{
    QDomNodeList list{configXml.elementsByTagName(XML_NAME_UPDATE)};
    QDomElement updateElement{list.at(0).toElement()};
    if (!updateElement.isNull())
        updatePolicy_ = static_cast<UpdatePolicy>(
            updateElement.attribute(XML_NAME_VALUE).toInt());

    list = configXml.elementsByTagName(XML_NAME_STYLE);
    QDomElement styleElement{list.at(0).toElement()};
    if (!styleElement.isNull())
        styleName_ = styleElement.attribute(XML_NAME_VALUE);

    list = configXml.elementsByTagName(XML_NAME_IMPORTPATH);
    QDomElement importPathElement{list.at(0).toElement()};
    if (!importPathElement.isNull())
        importFilePath_ = importPathElement.attribute(XML_NAME_VALUE);
}

QString Configuration::generateConfigXml() const
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

    return doc.toString();
}

bool Configuration::saveConfigXml(const QString& configXml)
{
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

    if (file.write(configXml.toStdString().c_str()) == -1)
    {
        LOG(LogTypes::CONFIG, "Error during config file save.");
        return false;
    }

    LOG(LogTypes::CONFIG, "Config saved.");
    return true;
}

void Configuration::setUpdatePolicy(bool alwaysCheck)
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
