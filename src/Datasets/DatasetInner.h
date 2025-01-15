#pragma once

#include <quazip/quazip.h>

#include "Dataset.h"
#include "DatasetUtilities.h"

class QuaZipFile;
class QTextStream;

class DatasetInner : public Dataset
{
    Q_OBJECT
public:
    explicit DatasetInner(const QString& name);

protected:
    std::tuple<bool, QVector<QVector<QVariant>>> getSample() override;

    std::tuple<bool, QVector<QVector<QVariant>>> getAllData() override;

    bool analyze() override;

    void closeZip() override;

private:
    bool openZip();

    static bool openQuaZipFile(QuaZipFile& zipFile);

    void retrieveColumnsFromXml(const QDomElement& root);

    void checkForTaggedColumn(const QDomElement& columnElement, Column column);

    bool fromXml(const QByteArray& definitionContent);

    static bool loadXmlFile(QByteArray& definitionContent, QuaZip& zip);

    bool loadStrings(QuaZip& zip);

    QVector<QVariant> fillRow(const QStringList& line, bool fillSamplesOnly);

    QVector<QVector<QVariant>> parseData(QTextStream& stream,
                                         bool fillSamplesOnly);

    std::tuple<bool, QVector<QVector<QVariant>>> fillData(QuaZip& zip,
                                                          bool fillSamplesOnly);

    void updateProgress(unsigned int currentRow, unsigned int rowCount,
                        unsigned int& lastEmittedPercent);

    static QVariant getElementAsVariant(ColumnType columnFormat,
                                        const QString& element);

    static QVariant getDefaultVariantForFormat(ColumnType format);

    QVector<QVector<QVariant>> prepareContainerForAllData() const;

    QVector<QVector<QVariant>> prepareContainerForSampleData() const;

    QuaZip zip_;

    const QString datasetsDir_{dataset_utilities::getDatasetsDir()};
};
