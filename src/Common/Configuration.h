#pragma once

#include <QString>

class QDomDocument;

class Configuration
{
public:
    Configuration();
    Configuration& operator=(const Configuration& other) = delete;
    Configuration& operator=(Configuration&& other) = default;

    ~Configuration() = default;

    Configuration(const Configuration& other) = delete;
    Configuration(Configuration&& other) = default;

    bool isUpdatePolicyPicked() const;

    bool needToCheckForUpdates() const;

    bool save() const;

    void setUpdatePolicy(bool alwaysCheck);

    QString getStyleName() const;

    void setStyleName(const QString& style);

    /// Public for unit tests.
    bool configValid() const;

    QString getImportFilePath() const;

    void setImportFilePath(const QString& path);

private:
    bool load();

    QString configDump() const;

    static bool loadConfigXml(QDomDocument& configXml);

    void parseConfigXml(const QDomDocument& configXml);

    QString generateConfigXml() const;

    static bool saveConfigXml(const QString& configXml);

    bool configValid_{false};

    enum class UpdatePolicy : signed char
    {
        NOT_DECIDED,
        ALWAYS_CHECK,
        NEVER_CHECK
    };

    QString styleName_;

    QString importFilePath_;

    UpdatePolicy updatePolicy_{UpdatePolicy::NOT_DECIDED};

    QString xmlNameConfig_{QStringLiteral("CONFIG")};
    QString xmlNameUpdate_{QStringLiteral("UPDATE")};
    QString xmlNameValue_{QStringLiteral("VALUE")};
    QString xmlNameStyle_{QStringLiteral("STYLE")};
    QString xmlNameImportPath_{QStringLiteral("IMPORTPATH")};
};
