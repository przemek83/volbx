#pragma once

#include <QString>

class QDomDocument;

class Configuration
{
public:
    Configuration();

    ~Configuration() = default;

    Configuration& operator=(const Configuration& other) = delete;
    Configuration(const Configuration& other) = delete;

    Configuration& operator=(Configuration&& other) = default;
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

    static const QString xmlNameConfig_;
    static const QString xmlNameUpdate_;
    static const QString xmlNameValue_;
    static const QString xmlNameStyle_;
    static const QString xmlNameImportPath_;
};
