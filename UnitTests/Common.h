#ifndef COMMON_H
#define COMMON_H

#include <QString>

/**
 * @brief common methods for unit tests.
 */
class Common
{
public:
    static QString loadFile(QString name);

    static void saveFile(QString fileName, QString& data);

    static QString getDefinitionDumpSuffix();

    static QString getDefinitionRefSuffix();

    static QString getDataTsvDumpSuffix();

private:
    Common();
    virtual ~Common();
    Q_DISABLE_COPY(Common)
};

#endif // COMMON_H
