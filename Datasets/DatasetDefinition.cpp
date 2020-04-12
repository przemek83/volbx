#include "DatasetDefinition.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QtXml/QDomDocument>

#include "Common/Constants.h"
#include "Shared/Logger.h"

#include "Dataset.h"

DatasetDefinition::DatasetDefinition(QString name)
    : name_(std::move(name)), emptyColName_(QObject::tr("no name"))
{
}

bool DatasetDefinition::isValid() const { return false; }

int DatasetDefinition::rowCount() const { return rowsCount_; }

int DatasetDefinition::columnCount() const { return columnsCount_; }

bool DatasetDefinition::isSpecialColumnTagged(SpecialColumn column) const
{
    return specialColumns_.contains(column);
}

std::tuple<bool, int> DatasetDefinition::getSpecialColumnIfExists(
    SpecialColumn columnTag) const
{
    if (isSpecialColumnTagged(columnTag))
    {
        return {true, specialColumns_[columnTag]};
    }

    return {false, Constants::NOT_SET_COLUMN};
}

QString DatasetDefinition::getColumnName(int column) const
{
    if (columnsCount_ - 1 >= column)
    {
        return headerColumnNames_[column];
    }

    Q_ASSERT(false);
    return QLatin1String("");
}

QString DatasetDefinition::dumpDatasetDefinition() const
{
    QString dump;
    for (int i = 0; i < columnsCount_; ++i)
    {
        dump += "Column " + QString::number(i) +
                " name=" + headerColumnNames_.at(i);
        dump += " format=" +
                QString::number(static_cast<int>(columnsFormat_.at(i)));
        if (!activeColumns_.isEmpty())
        {
            const QString notActive(QStringLiteral(" not active"));
            const QString active(QStringLiteral(" active"));
            dump += " active=" + (activeColumns_[i] ? active : notActive);
        }

        QMapIterator<SpecialColumn, int> it(specialColumns_);
        if (it.findNext(i))
        {
            dump += " special=" + QString::number(static_cast<int>(it.key()));
        }
        dump.append(QLatin1String("\n"));
    }
    return dump;
}

QString DatasetDefinition::getName() const { return name_; }

QString DatasetDefinition::getError() const { return error_; }

void DatasetDefinition::clearSampleData()
{
    sampleData_.clear();
    sampleData_.resize(0);
}

const QVector<QVector<QVariant> >* DatasetDefinition::getSampleData() const
{
    return &sampleData_;
}

void DatasetDefinition::setActiveColumns(const QVector<bool>& activeColumns)
{
    activeColumns_ = activeColumns;
}

const QVector<bool>* DatasetDefinition::getActiveColumns() const
{
    return &activeColumns_;
}

int DatasetDefinition::getActiveColumnCount() const
{
    return activeColumns_.count(true);
}

void DatasetDefinition::rebuildDefinitonUsingActiveColumnsOnly()
{
    QVector<DataFormat> tempColumnsFormat;

    QStringList tempHeaderColumnNames = QStringList();

    QMap<SpecialColumn, int> specialColumnsTemp = QMap<SpecialColumn, int>();

    int activeColumnNumber = 0;

    bool specialColumnDateMarked =
        isSpecialColumnTagged(SPECIAL_COLUMN_TRANSACTION_DATE);
    bool specialColumnPriceMarked =
        isSpecialColumnTagged(SPECIAL_COLUMN_PRICE_PER_UNIT);

    for (int i = 0; i < activeColumns_.count(); ++i)
    {
        if (activeColumns_.at(i))
        {
            tempColumnsFormat.push_back(columnsFormat_[i]);
            tempHeaderColumnNames << headerColumnNames_[i];

            if (specialColumnDateMarked &&
                specialColumns_.value(SPECIAL_COLUMN_TRANSACTION_DATE) == i)
            {
                specialColumnsTemp[SPECIAL_COLUMN_TRANSACTION_DATE] =
                    activeColumnNumber;
            }

            if (specialColumnPriceMarked &&
                specialColumns_.value(SPECIAL_COLUMN_PRICE_PER_UNIT) == i)
            {
                specialColumnsTemp[SPECIAL_COLUMN_PRICE_PER_UNIT] =
                    activeColumnNumber;
            }

            activeColumnNumber++;
        }
    }

    columnsFormat_ = tempColumnsFormat;
    headerColumnNames_ = tempHeaderColumnNames;
    specialColumns_ = specialColumnsTemp;

    // Set new column count.
    columnsCount_ = getActiveColumnCount();

    // Those info is not needed anymore.
    activeColumns_.clear();
}

void DatasetDefinition::setSpecialColumn(SpecialColumn columnTag, int column)
{
    specialColumns_[columnTag] = column;
}

QVariant DatasetDefinition::getDefaultVariantForFormat(
    const DataFormat format) const
{
    switch (format)
    {
        case DATA_FORMAT_STRING:
        {
            return QVariant(QVariant::Int);
        }

        case DATA_FORMAT_FLOAT:
        {
            return QVariant(QVariant::Double);
        }
        case DATA_FORMAT_DATE:
        {
            return QVariant(QVariant::Date);
        }

        case DATA_FORMAT_UNKNOWN:
        {
            Q_ASSERT(false);
            return QVariant(QVariant::String);
        }
    }
}

void DatasetDefinition::toXml(QByteArray& data, int rowCountNumber) const
{
    QDomDocument xmlDocument(DATASET_NAME);
    QDomElement root = xmlDocument.createElement(DATASET_NAME);
    xmlDocument.appendChild(root);

    // Columns.
    QDomElement columns = xmlDocument.createElement(DATASET_COLUMNS);
    root.appendChild(columns);

    for (int i = 0; i < columnsCount_; ++i)
    {
        QDomElement node = xmlDocument.createElement(DATASET_COLUMN);
        node.setAttribute(DATASET_COLUMN_NAME, headerColumnNames_.at(i));
        node.setAttribute(DATASET_COLUMN_FORMAT, columnsFormat_.at(i));

        QMapIterator<SpecialColumn, int> it(specialColumns_);
        if (it.findNext(i))
        {
            node.setAttribute(DATASET_COLUMN_SPECIAL_TAG,
                              QString::number(static_cast<int>(it.key())));
        }
        columns.appendChild(node);
    }

    // Add row count.
    QDomElement rowCount = xmlDocument.createElement(DATASET_ROW_COUNT);
    rowCount.setAttribute(DATASET_ROW_COUNT, QString::number(rowCountNumber));
    root.appendChild(rowCount);

    data = xmlDocument.toByteArray();
}
