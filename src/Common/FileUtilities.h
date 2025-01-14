#pragma once

#include <QString>

namespace FileUtilities
{
std::pair<bool, QString> loadFile(const QString& name);
}  // namespace FileUtilities
