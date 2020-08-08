#ifndef EXPORTVBX_H
#define EXPORTVBX_H

#include <QVector>

#include <ExportData.h>
#include <QHash>

class QAbstractItemModel;
class QAbstractItemView;
class QIODevice;
class QuaZipFile;

/**
 * @class ExportVbx
 * @brief Class for exporting data to vbx files.
 */
class ExportVbx : public ExportData
{
    Q_OBJECT
public:
    explicit ExportVbx(QObject* parent = nullptr);
    ~ExportVbx() override = default;

    bool generateVbx(const QAbstractItemView& view, QIODevice& ioDevice);

protected:
    bool writeContent(const QByteArray& content, QIODevice& ioDevice) override;

    QByteArray getEmptyContent() override;

    QByteArray generateHeaderContent(const QAbstractItemModel& model) override;

    QByteArray generateRowContent(const QAbstractItemModel& model, int row,
                                  int skippedRowsCount) override;

    QByteArray getContentEnding() override;

private:
    void variantToString(const QVariant& variant, QByteArray& destinationArray,
                         char separator);

    bool exportStrings(QuaZipFile& zipFile, const QByteArray& stringsContent);

    bool exportDefinition(QuaZipFile& zipFile, const QAbstractItemView& view,
                          int rowCount);

    static constexpr char separator_{';'};
    QHash<QString, int> stringsMap_;
    QByteArray stringsContent_;
    int nextIndex_{1};
    unsigned int lines_{0};
    static constexpr char newLine_{'\n'};
};

#endif  // EXPORTVBX_H
