#pragma once

#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QTabBar>

class QMainWindow;

class TabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit TabBar(QWidget* parent);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupLineEdit();

    QMainWindow* getCurrentTabWidget() const;

    QLineEdit nameEdit_;

    QRegularExpressionValidator validator_;

private slots:
    void editingNameFinished();
};
