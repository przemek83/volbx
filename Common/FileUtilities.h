#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H

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
std::pair<bool, QString> loadFile(QString name);
}  // namespace FileUtilities

#endif  // FILEUTILITIES_H
