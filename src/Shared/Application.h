#pragma once

#include <QString>

namespace Application
{
/// @brief Set version, application name, company name.
void setAdditionalApplicatioInfo(const char* productName);

void setCssStyle(const QString& styleName);

void setQtStyle(const QString& name);

void initStyle(const QString& nameFromConfig);
}  // namespace Application
