#ifndef APPLICATION_H
#define APPLICATION_H

#include <QString>

/**
 * Set of functions used to change application look or behaviour.
 */
namespace Application
{
/**
 * @brief Set version, application name, company name.
 * @param productName Name of product,
 */
void setAdditionalApplicatioInfo(const char* productName);

/**
 * @brief Set application style using css style file.
 * @param styleName Name of style file.
 */
void setCssStyle(const QString& styleName);

/**
 * @brief Set Qt inner style.
 * @param name Name of inner style.
 */
void setQtStyle(const QString& name);

/**
 * @brief initStyle Initialize css style to one passed as parameter.
 * @param nameFromConfig Name of style retrieved from config.
 */
void initStyle(const QString& nameFromConfig);
}  // namespace Application

#endif  // APPLICATION_H
