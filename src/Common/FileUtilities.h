#pragma once

#include <QString>

/**
 * Functions related to files.
 */
namespace FileUtilities
{
/**
 * @brief Load given file.
 * @param name File name.
 * @return Bool indicating success and content.
 */
std::pair<bool, QString> loadFile(const QString& name);
}  // namespace FileUtilities
