#pragma once

#include <memory>

#include <QObject>

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
private slots:
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
     * @brief check export for given dataset name.
     * @param fileName name of dataset.
     */
    void checkExport(const QString& datasetName, QBuffer& exportedBuffer);

    void addTestCases(const QString& testNamePrefix);

    QByteArray loadDataFromZip(QuaZip& zip, const QString& fileName);

    void checkExportedData(const QString& fileName, QuaZip& zipOriginal,
                           QuaZip& zipGenerated);

    void checkExportedDefinitions(QuaZip& zipOriginal, QuaZip& zipGenerated);

    static const QVector<QString> testFileNames_;
};
