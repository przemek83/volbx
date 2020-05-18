#ifndef IMPORTTAB_H
#define IMPORTTAB_H

#include <memory>

#include <QWidget>

class Dataset;

/**
 * @brief Import tabs base class.
 */
class ImportTab : public QWidget
{
    Q_OBJECT
public:
    explicit ImportTab(QWidget* parent = nullptr);

    ~ImportTab() override = default;

    virtual std::unique_ptr<Dataset> getDataset() = 0;

Q_SIGNALS:
    void definitionIsReady(bool);
};

#endif  // IMPORTTAB_H
