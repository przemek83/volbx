#pragma once

#include <QObject>

class QTableView;

class SpreadsheetsTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase() const;

    void testDefinition_data() const;
    static void testDefinition();

    void testData_data() const;
    static void testData();

    static void testDamagedFiles_data();
    static void testDamagedFiles();

    void compareExpectedDefinitionsOfOdsAndXlsx_data() const;
    static void compareExpectedDefinitionsOfOdsAndXlsx();

    void compareExpectedTsvDumpsOfOdsAndXlsx_data() const;
    static void compareExpectedTsvDumpsOfOdsAndXlsx();

private:
    void addTestCaseForOdsAndXlsxComparison(
        const QString& testNamePrefix) const;

    static void compareOdsAndXlsxExpectedData(const QString& fileSuffix);

    void generateExpectedData() const;

    static void addTestCasesForFileNames(const QVector<QString>& fileNames);

    const QVector<QString> testFileNames_{
        "excel",   "HistVsNormal", "import1",
        "import2", "import3",      "smallDataSet",
        "test2",   "testAccounts", "testDataWithPlot",
        "test",    "testFileData", "test01",
        "test03",  "test04"};
};
