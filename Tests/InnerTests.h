#ifndef INNERTESTS_H
#define INNERTESTS_H

#include <QFileInfo>
#include <QObject>
#include <QTableView>

class DatasetDefinitionInner;
class DatasetInner;

/**
 * @brief Unit test of inner format functionalities.
 */
class InnerTests : public QObject
{
    Q_OBJECT
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

    QString tempFilename_;

    /**
     * @brief compare definition files
     * @param original original definition file.
     * @param generated generated definition file.
     */
    void compareDefinitionFiles(QByteArray& original, QByteArray& generated);

private slots:
    void initTestCase();

    /**
     * @brief test all datasets.
     */
    void testDatasets();

    /**
     * @brief test int details one definition file including export.
     */
    void testPartialData();

    /**
     * @brief cleanup temp file.
     */
    void cleanupTestCase();
};

#endif  // INNERTESTS_H
