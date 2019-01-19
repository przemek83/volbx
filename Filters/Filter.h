#ifndef FILTER_H
#define FILTER_H

#include <QGroupBox>

/**
 * @brief Filters base class.
 */
class Filter : public QGroupBox
{
    Q_OBJECT
public:
    Filter(QString title, int column, QWidget *parent = 0);

    virtual ~Filter();

public slots:
    virtual void setChecked(bool checked);

protected:
    int column_;

private:
    Q_DISABLE_COPY(Filter)
};

#endif // FILTER_H
