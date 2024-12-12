#pragma once

#include <QString>

class QDomDocument;

/**
 * @brief Singleton for configuration.
 */
class Configuration
{
public:
    Configuration& operator=(const Configuration& other) = delete;
    Configuration(const Configuration& other) = delete;

    Configuration& operator=(Configuration&& other) = delete;
    Configuration(Configuration&& other) = delete;

    /**
     * @brief Used to access configuration singleton.
     * @return Singleton instance.
     */
    static Configuration& getInstance();

    /**
     * @brief Check if update policy was picked in past.
     * @return True is picked, false if not.
     */
    bool isUpdatePolicyPicked() const;

    /**
     * @brief Check if application should check for updates.
     * @return True if should check, false otherwise.
     */
    bool needToCheckForUpdates() const;

    /**
     * @brief Save configuration into file.
     * @return True if success, false otherwise.
     */
    bool save() const;

    /**
     * @brief Set update policy.
     * @param alwaysCheck True is should always check.
     */
    void setUpdatePolicy(bool alwaysCheck);

    QString getStyleName() const;

    void setStyleName(const QString& style);

    /// For unit tests.
    bool configValid() const;

    /**
     * @brief Load/reload configuration. Public for unit tests.
     * @return true if loading finished with success.
     */
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
