#ifndef DATASETDEFINITIONSPREADSHEET_H
#define DATASETDEFINITIONSPREADSHEET_H

#include <QHash>

#include <quazip.h>

#include "DatasetDefinition.h"

/**
 * @brief Dataset definition for spreadsheet files.
 */
class DatasetDefinitionSpreadsheet : public DatasetDefinition
{
public:
    DatasetDefinitionSpreadsheet(const QString& name, QString& zipFileName);

    virtual ~DatasetDefinitionSpreadsheet();

    virtual bool isValid() const;

    bool init();

    virtual bool getData(QVector<QVector<QVariant> >* dataContainer);

    virtual QVariant* getSharedStringTable();

protected:
    virtual void updateSampleDataStrings();

    virtual const QString& getSheetName() = 0;

    virtual bool getDataFromZip( QuaZip& zip,
                                 const QString& sheetName,
                                 QVector<QVector<QVariant> >* dataContainer,
                                 bool fillSamplesOnly ) = 0;

    bool checkCorrectness(QuaZip& zip) const;

    virtual bool getSheetList(QuaZip& zip) = 0;

    virtual bool getColumnList(QuaZip& zip,
                               const QString& sheetName) = 0;

    virtual bool loadSpecificData(QuaZip& zip) = 0;

    virtual bool getColumnTypes(QuaZip& zip,
                                const QString& sheetName) = 0;

    ///Next index to be used in strings hash map.
    int nextIndex_;

    ///Temporary string <-> index map used to build shared string table.
    QHash<QString, int> stringsMap_;

    /**
     *  Get index in map for given string.
     *  @param string string to search in map.
     *  @return index of string in map.
     */
    inline int getStringIndex(const QString& string)
    {
        int& index = stringsMap_[string];
        if( 0 == index )
        {
            index = nextIndex_;
        }
        nextIndex_++;
        return index;
    }

    ///Zip file.
    QuaZip zip_;

private:
    Q_DISABLE_COPY(DatasetDefinitionSpreadsheet)
};

#endif // DATASETDEFINITIONSPREADSHEET_H
