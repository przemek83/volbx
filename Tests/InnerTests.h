#pragma once

#include <memory>

#include <QObject>

class Dataset;
class QTableView;

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

    void cleanupTestCase();

private:
    void generateDumpData();

    void checkDatasetDefinition(const QString& fileName,
                                const std::unique_ptr<Dataset>& dataset) const;

    void checkDatasetData(const QString& fileName,
                          const QTableView& view) const;

    /**
     * @brief check export for given dataset name.
     * @param fileName name of dataset.
     */
    void checkExport(QString fileName);

    /**
     * @brief compare definition files
     * @param original original definition file.
     * @param generated generated definition file.
     */
    void compareDefinitionFiles(QByteArray& original, QByteArray& generated);

    void addTestCases(const QString& testNamePrefix);

    const QString tempFilename_{"temp"};

    static const QVector<QString> testFileNames_;
};
