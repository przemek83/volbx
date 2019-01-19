#ifndef MAINTABWIDGET_H
#define MAINTABWIDGET_H

#include <QTabWidget>
#include <QDate>

class TableModel;
class DataView;
class FilteringProxyModel;
class MainTab;
class ViewDockWidget;

/**
 * @brief TabWidget. Each tab contains data and plots.
 */
class MainTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit MainTabWidget(QWidget *parent = 0);

    virtual ~MainTabWidget();

    FilteringProxyModel* getCurrentProxyModel();

    TableModel* getCurrentDataModel();

    DataView* getCurrentDataView();

private:
    Q_DISABLE_COPY(MainTabWidget)

    void changeDataViewMode(DataView* view);

    ViewDockWidget* getCurrentDataViewDock();

    MainTab* getCurrentMainTab();

private slots:
    void setTextFilterInProxy(int column, QSet<QString> bannedStrings);

    void setDateFilterInProxy(int column,
                              QDate from,
                              QDate to,
                              bool filterEmptyDates);

    void setNumericFilterInProxy(int column, double from, double to);

    void addBasicPlot();

    void addHistogramPlot();

    void addGroupingPlot();
};

#endif // MAINTABWIDGET_H
