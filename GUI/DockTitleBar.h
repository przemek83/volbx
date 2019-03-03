#ifndef DOCKTITLEBAR_H
#define DOCKTITLEBAR_H

#include <QWidget>

class QPushButton;

namespace Ui {
    class DockTitleBar;
} // namespace Ui

/**
 * @brief Title bar for dockwidgets with some buttons.
 */
class DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit DockTitleBar(QWidget *parent = nullptr);

    ~DockTitleBar() override;

    QPushButton* getCloseButton();

    QPushButton* getFloatingButton();

    QPushButton* getSelectAllButton();

    QPushButton* getUnselectAllButton();

    QPushButton* getExportButton();

    QPushButton* getResetButton();

    void setTitle(QString titleText);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Q_DISABLE_COPY(DockTitleBar)

    Ui::DockTitleBar *ui;
};

#endif // DOCKTITLEBAR_H
