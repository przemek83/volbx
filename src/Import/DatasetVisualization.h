#pragma once

#include <memory>

#include <QWidget>

#include <Common/ColumnTag.h>
#include <Datasets/Dataset.h>

#include "ui_DatasetVisualization.h"

class QTreeWidgetItem;
class QComboBox;

class DatasetVisualization : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetVisualization(QWidget* parent = nullptr);

    void setDataset(std::unique_ptr<Dataset> dataset);

    void clear();

    std::unique_ptr<Dataset> retrieveDataset();

public Q_SLOTS:
    void selectCurrentColumn(int column);

private:
    static void setCurrentIndexUsingColumn(QComboBox* combo, int column);

    void setupColumnsListWidget();

    void setTaggedColumns();

    QVector<bool> getActiveColumns() const;

    void setTaggedColumnInDataset(ColumnTag tag, QComboBox* combo);

    QString getTypeDisplayNameForGivenColumn(int column) const;

    void fillTaggedColumnCombos();

    void setAllItemsInColumnsListToState(Qt::CheckState state);

    static int getCurrentValueFromCombo(QComboBox* combo);

    std::unique_ptr<Ui::DatasetVisualization> ui_;

    const QString typeNameString_{tr("Name")};
    const QString typeNameFloat_{tr("Number")};
    const QString typeNameDate_{tr("Date")};

    std::unique_ptr<Dataset> dataset_{nullptr};

private Q_SLOTS:
    void searchTextChanged(const QString& newText);

    void currentColumnOnTreeChanged(QTreeWidgetItem* current,
                                    QTreeWidgetItem* previous);

    void selectAllClicked();

    void unselectAllClicked();

    void refreshColumnList(int newIndex);

Q_SIGNALS:
    void currentColumnNeedSync(int currentColumn);
};
