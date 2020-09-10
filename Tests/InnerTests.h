#pragma once

#include <memory>

#include <QObject>
#include <QVector>

class Dataset;
class QTableView;
class QBuffer;
class QuaZip;

/**
 * @brief Test for inner format functionalities.
 */
class InnerTests : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testDefinition_data();
    void testDefinition();

    void testData_data();
    void testData();

    void testExport_data();
    void testExport();

    void testPartialData();

private:
    void generateDumpData();

    /**
     * @brief Check export for given dataset name.
     * @param datasetName Name of dataset.
     * @param exportedBuffer Exported zip.
     */
    void checkExport(const QString& datasetName, QBuffer& exportedBuffer);

    void addTestCases(const QString& testNamePrefix);

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
