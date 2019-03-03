#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui {
    class About;
}

/**
 * @brief The About dialog class.
 */
class About : public QDialog
{
    Q_OBJECT
public:
    explicit About(QWidget *parent = nullptr);

    virtual ~About();

private:
    Q_DISABLE_COPY(About)

    Ui::About *ui;
};

#endif // ABOUT_H
