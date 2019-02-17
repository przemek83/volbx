#ifndef APPLICATION_H
#define APPLICATION_H

#include <QString>

/**
 * @brief Set of static methods used to change application look or behaviour.
 */
class Application
{
public:
    ///Set version, application name, company name.
    static void setAdditionalApplicatioInfo(const char* productName);

    /**
     * @brief set aplication style using css style file.
     * @param styleName name of style file.
     */
    static void setCssStyle(QString styleName);

    /**
     * @brief set Qt inner style.
     * @param name name of inner style.
     */
    static void setQtStyle(QString name);

    static QString getGroupBoxStyle();

    static void initStyle(QString nameFromConfig);

private:
    Application() = default;
    virtual ~Application() = default;
    Q_DISABLE_COPY(Application)

    static QString getStylePath(QString styleName);

    static QString getResStylePath(QString styleName);

    static QString getLocalStyleFilePath(QString styleName);

    static void clearAppFocus();

    static const char* cssSuffix_;
};

#endif // APPLICATION_H
