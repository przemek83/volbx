#pragma once

#include <QString>

class QDomDocument;

class Configuration
{
public:
    Configuration& operator=(const Configuration& other) = delete;
    Configuration(const Configuration& other) = delete;

    Configuration& operator=(Configuration&& other) = delete;
    Configuration(Configuration&& other) = delete;

    static Configuration& getInstance();

    bool isUpdatePolicyPicked() const;

    bool needToCheckForUpdates() const;

    bool save() const;

    void setUpdatePolicy(bool alwaysCheck);

    QString getStyleName() const;

    void setStyleName(const QString& style);

    /// For unit tests.
    bool configValid() const;

    /// Public for unit tests.
    bool load();

    QString getImportFilePath() const;

    void setImportFilePath(const QString& path);

private:
    Configuration();
    ~Configuration() = default;

    QString configDump() const;

    static bool loadConfigXml(QDomDocument& configXml);

    void parseConfigXml(QDomDocument& configXml);

    QString generateConfigXml() const;

    static bool saveConfigXml(const QString& configXml);

    bool configValid_{false};

    enum class UpdatePolicy : unsigned char
    {
        NOT_DECIDED,
        ALWAYS_CHECK,
        NEVER_CHECK
    };

    QString styleName_;

    QString importFilePath_;

    UpdatePolicy updatePolicy_{UpdatePolicy::NOT_DECIDED};

    const QString XML_NAME_CONFIG{QStringLiteral("CONFIG")};
    const QString XML_NAME_UPDATE{QStringLiteral("UPDATE")};
    const QString XML_NAME_VALUE{QStringLiteral("VALUE")};
    const QString XML_NAME_STYLE{QStringLiteral("STYLE")};
    const QString XML_NAME_IMPORTPATH{QStringLiteral("IMPORTPATH")};
};
