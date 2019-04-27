#ifndef DATASETDEFINITIONINNER_H
#define DATASETDEFINITIONINNER_H

#include <quazip5/quazip.h>

#include "DatasetDefinition.h"

/**
 * @brief Dataset definition for inner format files.
 */
class DatasetDefinitionInner : public DatasetDefinition
{
public:
    explicit DatasetDefinitionInner(const QString& name);

    ~DatasetDefinitionInner() override;

    DatasetDefinitionInner& operator=(const DatasetDefinitionInner& other) = delete;
    DatasetDefinitionInner(const DatasetDefinitionInner& other) = delete;

    DatasetDefinitionInner& operator=(DatasetDefinitionInner&& other) = delete;
    DatasetDefinitionInner(DatasetDefinitionInner&& other) = delete;

    bool isValid() const override;

    QVariant* getSharedStringTable() override;

    bool getData(QVector<QVector<QVariant> >* dataContainer);

protected:
    void updateSampleDataStrings() override;

private:
    bool fillData(QuaZip& zip,
                  QVector<QVector<QVariant> >* dataContainer,
                  bool fillSamplesOnly);

    ///Load definition, strings and semple data.
    bool load();

    //Parse given xml and fill inner definition containers.
    bool fromXml(QByteArray& definitionContent);

    ///Load definition file from zip.
    bool loadXmlFile(QByteArray& definitionContent, QuaZip& zip);

    ///Load strings from zip file.
    bool loadStrings(QuaZip& zip);

    ///Array with strings.
    QVariant* stringsTable_;

    ///Zip file.
    QuaZip zip_;

    ///Add current element into given container.
    void addElementToContainer(const DataFormat columnFormat,
                               const QString& element,
                               QVector<QVector<QVariant> >* dataContainer,
                               const int lineCounter,
                               const int columnToFill) const;
};

#endif // DATASETDEFINITIONINNER_H
