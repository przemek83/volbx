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
    explicit Filter(const QString& title, QWidget* parent = nullptr);

    ~Filter() override = default;

    Filter& operator=(const Filter& other) = delete;
    Filter(const Filter& other) = delete;

    Filter& operator=(Filter&& other) = delete;
    Filter(Filter&& other) = delete;
};

#endif // FILTER_H
