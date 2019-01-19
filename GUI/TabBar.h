#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include <QLineEdit>

/**
 * @brief Custom tab bar for tab widget.
 */
class TabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit TabBar(QWidget *parent = 0);

    virtual ~TabBar();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

    bool eventFilter(QObject *obj, QEvent *event);

private:
    Q_DISABLE_COPY(TabBar)

    ///Line edit for changing tab name.
    QLineEdit lineEdit_;

private slots:
    void editingOfnameFinished();
};

#endif // TABBAR_H
