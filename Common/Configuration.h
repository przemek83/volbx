#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>

/**
 * @brief Sigleton for configuration.
 */
class Configuration
{
public:
    Configuration& operator=(const Configuration& other) = delete;
    Configuration(const Configuration& other) = delete;

    Configuration& operator=(Configuration&& other) = delete;
    Configuration(Configuration&& other) = delete;

    /**
     * Used to access Config singleton.
     * @return singleton instance.
     */
    static Configuration& getInstance();

    bool needToShowUpdatePickerDialog() const;

    bool needToCheckForUpdates() const;

    /**
     * Save config into file.
     * @return true if success.
     */
    bool save();

    void setUpdatesCheckingOption(bool alwaysCheck);

    QString getStyle() const;

    void setStyle(QString style);

    ///For unit tests.
    bool configWasValid() const;

    /**
     * Load/reload Config. Public for unit tests.
     * @return true if loading finished with success.
     */
    bool load();

    QString getImportFilePath() const;

    void setImportFilePath(QString path);

private:
    Configuration();
    ~Configuration() = default;

    ///Flag indicating that config existed and was valid.
    bool configValid_ {false};

    enum UpdateOption
    {
        UPDATES_CHOICE_NOT_PICKED,
        UPDATES_ALWAYS_CHECK,
        UPDATES_NEVER_CHECK
    };

    QString style_;

    QString importFilePath_;

    ///Names used in xml.
    const static char* xmlNames_[];

    UpdateOption updateOption_ {UPDATES_CHOICE_NOT_PICKED};

    ///Enum used for xml field names.
    enum XmlNames
    {
        XML_NAME_CONFIG,
        XML_NAME_UPDATE,
        XML_NAME_VALUE,
        XML_NAME_STYLE,
        XML_NAME_IMPORTPATH
    };

    /**
     * Get viewable for of config.
     * @return config in readable text form.
     */
    QString configDump() const;
};

#endif // CONFIGURATION_H
