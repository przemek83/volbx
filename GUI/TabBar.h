#pragma once

#include <QLineEdit>
#include <QTabBar>

/**
 * @brief Tab bar for tab widget.
 */
class TabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit TabBar(QWidget* parent = nullptr);

    ~TabBar() override = default;

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupLineEdit();

    /// Line edit for changing tab name.
    QLineEdit lineEdit_;

private Q_SLOTS:
    void editingOfnameFinished();
};
