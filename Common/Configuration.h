#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>

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

    bool needToShowUpdatePickerDialog() const;

    bool needToCheckForUpdates() const;

    /**
     * Save configuration into file.
     * @return true if success.
     */
    bool save();

    void setUpdatesCheckingOption(bool alwaysCheck);

    QString getStyle() const;

    void setStyle(const QString& style);

    /// For unit tests.
    bool configWasValid() const;

    /**
     * Load/reload configuration. Public for unit tests.
     * @return true if loading finished with success.
     */
    bool load();

    QString getImportFilePath() const;

    void setImportFilePath(const QString& path);

private:
    Configuration();
    ~Configuration() = default;

    /**
     * Get viewable for of configuration.
     * @return configuration in readable text form.
     */
    QString configDump() const;

    /// Flag indicating that configuration existed and was valid.
    bool configValid_{false};

    enum UpdateOption
    {
        UPDATES_CHOICE_NOT_PICKED,
        UPDATES_ALWAYS_CHECK,
        UPDATES_NEVER_CHECK
    };

    QString style_;

    QString importFilePath_;

    UpdateOption updateOption_{UPDATES_CHOICE_NOT_PICKED};

    const QString XML_NAME_CONFIG{QStringLiteral("CONFIG")};
    const QString XML_NAME_UPDATE{QStringLiteral("UPDATE")};
    const QString XML_NAME_VALUE{QStringLiteral("VALUE")};
    const QString XML_NAME_STYLE{QStringLiteral("STYLE")};
    const QString XML_NAME_IMPORTPATH{QStringLiteral("IMPORTPATH")};
};

#endif  // CONFIGURATION_H
