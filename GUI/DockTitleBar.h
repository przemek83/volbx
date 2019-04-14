#ifndef DOCKTITLEBAR_H
#define DOCKTITLEBAR_H

#include <QWidget>

class QPushButton;

namespace Ui
{
class DockTitleBar;
} // namespace Ui

/**
 * @brief Title bar for dockwidgets with some buttons.
 */
class DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit DockTitleBar(QWidget* parent = nullptr);

    ~DockTitleBar() override;

    DockTitleBar& operator=(const DockTitleBar& other) = delete;
    DockTitleBar(const DockTitleBar& other) = delete;

    DockTitleBar& operator=(DockTitleBar&& other) = delete;
    DockTitleBar(DockTitleBar&& other) = delete;

    QPushButton* getCloseButton();

    QPushButton* getFloatingButton();

    QPushButton* getSelectAllButton();

    QPushButton* getUnselectAllButton();

    QPushButton* getExportButton();

    QPushButton* getResetButton();

    void setTitle(const QString& titleText);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::DockTitleBar* ui;
};

#endif // DOCKTITLEBAR_H
