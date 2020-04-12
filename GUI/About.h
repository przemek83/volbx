#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui
{
class About;
}  // namespace Ui

/**
 * @brief The About dialog class.
 */
class About : public QDialog
{
    Q_OBJECT
public:
    explicit About(QWidget* parent = nullptr);

    ~About() override;

    About& operator=(const About& other) = delete;
    About(const About& other) = delete;

    About& operator=(About&& other) = delete;
    About(About&& other) = delete;

private:
    Ui::About* ui;
};

#endif  // ABOUT_H
