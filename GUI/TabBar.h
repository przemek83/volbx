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
    explicit TabBar(QWidget* parent = nullptr);

    ~TabBar() override = default;

    TabBar& operator=(const TabBar& other) = delete;
    TabBar(const TabBar& other) = delete;

    TabBar& operator=(TabBar&& other) = delete;
    TabBar(TabBar&& other) = delete;

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    bool eventFilter(QObject* obj, QEvent* event);

private:
    ///Line edit for changing tab name.
    QLineEdit lineEdit_;

private slots:
    void editingOfnameFinished();
};

#endif // TABBAR_H
