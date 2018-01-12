#ifndef TRAFFICWINDOW_H
#define TRAFFICWINDOW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

using namespace QtCharts;

class QAbstractItemModel;
class QAbstractItemView;

class TrafficWindow : public QWidget
{
    Q_OBJECT

public:
    TrafficWindow();
    void setData(QList<QStringList> &tableData,
                 QMap<int, QList<double_t>> &graphData);
    void setTitle(QString &title);

private slots:
    void nodeClicked(QModelIndex index);

private:
    void setupModel();
    void setupViews();
    void setupSeries();

    QChart *chart;
    QChartView *lineChart;
    QAbstractItemModel *model;
    QLineSeries *trafficSeries;
    QMap<int, QLineSeries*> trafficSeriesList;
};

#endif // TRAFFICWINDOW_H
