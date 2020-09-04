#pragma once

#include <QWidget>

class QPushButton;

namespace Ui
{
class DockTitleBar;
}  // namespace Ui

/**
 * @brief Title bar with buttons used in dock windows.
 */
class DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit DockTitleBar(QWidget* parent = nullptr);

    ~DockTitleBar() override;

    void setTitle(const QString& titleText);

    enum class Button : char
    {
        CLOSE,
        FLOATING,
        SELECT_ALL,
        UNSELECT_ALL,
        EXPORT,
        RESET
    };

    void setButtonVisible(Button button, bool visible);

    void setButtonEnabled(Button button, bool enabled);

signals:
    void closeClicked();
    void floatingClicked();
    void selectAllClicked();
    void unselectAllClicked();
    void exportClicked();
    void resetClicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void connectButtons();

    void drawBorder();

    QPushButton* getButton(Button button) const;

    Ui::DockTitleBar* ui;
};
