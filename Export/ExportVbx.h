#pragma once

#include <QVector>

#include <ExportData.h>
#include <QHash>

#include <Qt5Quazip/quazip.h>

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

    /**
     * @brief Generate inner Volbx format of data (.vbx).
     * @param view View with selected data to export.
     * @param ioDevice Device to write to.
     * @return True on success, false otherwise.
     */
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

    bool exportStrings(QIODevice& ioDevice);

    bool exportDefinition(const QAbstractItemView& view,
                          QIODevice& ioDevice) const;

    static bool write(QIODevice& ioDevice, const QString& fileName,
                      const QByteArray& data, QuaZip::Mode mode);

    static constexpr char separator_{';'};
    QHash<QString, int> stringsMap_;
    QByteArray stringsContent_;
    int nextIndex_{1};
    unsigned int lines_{0};
    static constexpr char newLine_{'\n'};
};
