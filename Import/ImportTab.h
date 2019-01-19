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
    explicit ImportTab(QWidget *parent = 0);

    virtual ~ImportTab();

    virtual DatasetDefinition* getDatasetDefinition() = 0;

private:
    Q_DISABLE_COPY(ImportTab)

signals:
    void definitionIsReady(bool);
};

#endif // IMPORTTAB_H
