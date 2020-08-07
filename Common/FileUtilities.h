#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H

#include <QString>

/**
 * Functions related to files.
 */
namespace FileUtilities
{
std::pair<bool, QString> loadFile(QString name);
}

#endif  // FILEUTILITIES_H
