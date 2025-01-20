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
    DockTitleBar();

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

    void setButtonVisible(Button button, bool visible) const;

    void setButtonEnabled(Button button, bool enabled) const;

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
    void connectButtons() const;

    void drawBorder();

    QPushButton* getButton(Button button) const;

    std::unique_ptr<Ui::DockTitleBar> ui_{std::make_unique<Ui::DockTitleBar>()};
};
