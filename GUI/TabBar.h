#pragma once

#include <QLineEdit>
#include <QTabBar>

class QMainWindow;
/**
 * @brief Tab bar for tab widget.
 */
class TabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit TabBar(QWidget* parent = nullptr);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupLineEdit();

    QMainWindow* getCurrentTabWidget() const;

    QLineEdit nameEdit_;

private Q_SLOTS:
    void editingNameFinished();
};
