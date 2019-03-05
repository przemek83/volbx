#ifndef IMPORTTAB_H
#define IMPORTTAB_H

#include <QWidget>

class DatasetDefinition;

/**
 * @brief Import tabs base class.
 */
class ImportTab : public QWidget
{
    Q_OBJECT
public:
    explicit ImportTab(QWidget* parent = nullptr);

    ~ImportTab() override = default;

    ImportTab& operator=(const ImportTab& other) = delete;
    ImportTab(const ImportTab& other) = delete;

    ImportTab& operator=(ImportTab&& other) = delete;
    ImportTab(ImportTab&& other) = delete;

    virtual DatasetDefinition* getDatasetDefinition() = 0;

Q_SIGNALS:
    void definitionIsReady(bool);
};

#endif // IMPORTTAB_H
