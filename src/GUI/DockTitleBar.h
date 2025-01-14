#pragma once

#include <memory>

#include <QWidget>

#include "ui_DockTitleBar.h"

class QPushButton;

/// @brief Title bar with buttons used in dock windows.
class DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit DockTitleBar(QWidget* parent = nullptr);

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

Q_SIGNALS:
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

    std::unique_ptr<Ui::DockTitleBar> ui_;
};
