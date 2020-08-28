#pragma once

#include <QObject>

class DatasetInner;
class QTableView;

/**
 * @brief Test for inner format functionalities.
 */
class InnerTests : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void testDatasets();

    void testPartialData();

    void cleanupTestCase();

private:
    void generateDumpData();

    void checkDatasetDefinition(const QString& fileName,
                                DatasetInner* dataset) const;

    void checkDatasetData(const QString& fileName,
                          const QTableView& view) const;

    /**
     * @brief check import of given inner dataset.
     * @param fileName dataset name used as file name.
     * @param definition dataset definition.
     * @param view view from which data will be extracted.
     */
    void checkImport(const QString& fileName, DatasetInner* dataset,
                     const QTableView& view);

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

    const QString tempFilename_{"temp"};
};
