#pragma once

#include <Dataset.h>

class DatasetDummy : public Dataset
{
    Q_OBJECT
public:
    explicit DatasetDummy(const QString& name, QObject* parent = nullptr);

protected:
    bool analyze() override;

    std::tuple<bool, QVector<QVector<QVariant>>> getSample() override;

    std::tuple<bool, QVector<QVector<QVariant>>> getAllData() override;

    void closeZip() override;
};
