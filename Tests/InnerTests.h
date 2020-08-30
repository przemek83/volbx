#pragma once

#include <memory>

#include <QObject>

class Dataset;
class QTableView;
class QBuffer;

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

    const QString tempFilename_{"temp"};

    static const QVector<QString> testFileNames_;
};
