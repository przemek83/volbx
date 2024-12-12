#include "FileUtilities.h"

#include <QFile>
#include <QTextStream>

namespace FileUtilities
{
std::pair<bool, QString> loadFile(const QString& name)
{
    QFile file(name);

    if (!file.open(QIODevice::ReadOnly))
        return {false, ""};

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    return {true, stream.readAll()};
}
}  // namespace FileUtilities
