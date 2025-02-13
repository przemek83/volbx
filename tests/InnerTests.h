#pragma once

#include <QObject>
#include <QVector>

class Dataset;
class QTableView;
class QBuffer;
class QuaZip;

class InnerTests : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void testDefinition_data();
    static void testDefinition();

    void testData_data();
    static void testData();

    void testExport_data();
    static void testExport();

    static void testPartialData();

private:
    void generateDumpData();

    /**
     * @brief Check export for given dataset name.
     * @param datasetName Name of dataset.
     * @param exportedBuffer Exported zip.
     */
    static void checkExport(const QString& datasetName,
                            QBuffer& exportedBuffer);

    void addTestCases(const QString& testNamePrefix) const;

    static QByteArray loadDataFromZip(QuaZip& zip, const QString& fileName);

    static void checkExportedData(const QString& fileName, QuaZip& zipOriginal,
                                  QuaZip& zipGenerated);

    static void checkExportedDefinitions(QuaZip& zipOriginal,
                                         QuaZip& zipGenerated);

    static void generateVbxFile(const QString& datasetName, QBuffer& buffer,
                                const QVector<bool>& activeColumns);

    const QVector<QString> testFileNames_{
        "ExampleData", "po0_dmg", "po0_dmg2_bez_dat",
        "po0",         "po1",     "pustePola"};
};
