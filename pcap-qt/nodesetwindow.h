#ifndef NODESETWINDOW_H
#define NODESETWINDOW_H

#include <QWidget>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QChartView>
#include <QStandardItemModel>

using namespace QtCharts;

class NodeSetWindow : public QWidget
{
    Q_OBJECT

public:
    NodeSetWindow();
    void setData(QList<QStringList> &data);
    void setTitle(QString &title);

private:
    void setupModel();
    void setupViews();

    QChart *chart;
    QChartView *barChart;
    QAbstractItemModel *model;
};

#endif // NODESETWINDOW_H
