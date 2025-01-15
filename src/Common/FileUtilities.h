#pragma once

#include <QString>

namespace file_utilities
{
std::pair<bool, QString> loadFile(const QString& name);
}  // namespace file_utilities
