#pragma once

#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QTabBar>

#include <Common/DatasetUtilities.h>

class QMainWindow;

class TabBar : public QTabBar
{
    Q_OBJECT
public:
    TabBar();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupLineEdit();

    QMainWindow* getCurrentTabWidget() const;

    QLineEdit nameEdit_;

    QRegularExpressionValidator validator_{
        QRegularExpression(dataset_utilities::getDatasetNameRegExp())};

private slots:
    void editingNameFinished();
};
