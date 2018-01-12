#include "trafficwindow.h"
#include <QtWidgets>
#include <QHBoxLayout>
#include <QtCharts/QChart>
#include <iostream>

using namespace QtCharts;

const int interval = 10;    // データ間の秒数

TrafficWindow::TrafficWindow()
{
    setupModel();   // モデルの生成
    setupViews();   // ビューの生成と設定

    setWindowTitle(tr("トラフィック量"));  // ウィンドウタイトル
    resize(1005,600);    // ウィンドウサイズ
}

void TrafficWindow::setTitle(QString &title)
{
    setWindowTitle(title);
}

void TrafficWindow::setupModel()
{
    // テーブルモデルの生成
    model = new QStandardItemModel(0,3, this);
    model->setHeaderData(0, Qt::Horizontal, tr("送信元"));
    model->setHeaderData(1, Qt::Horizontal, tr("宛先"));
    model->setHeaderData(2, Qt::Horizontal, tr("トラフィック量"));
}

void TrafficWindow::setupViews()
{
    QTableView *table = new QTableView; // テーブルビューの生成
    table->setModel(model); // テーブルビューへのモデルの指定

    // グラフを生成
    chart = new QChart();
    chart->setTitle("トラフィック量 [Byte/sec]");

    // 折れ線グラフを生成
    lineChart = new QChartView();
    lineChart->setChart(chart);
    lineChart->setRenderHint(QPainter::Antialiasing);

    // レイアウトの設定
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(table);
    mainLayout->addWidget(lineChart);

    // テーブルを編集不可にする
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // セルクリック時に行を選択するように設定
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    // テーブルがクリックされたときの処理を設定
    connect(table, SIGNAL(clicked(QModelIndex)),
            this, SLOT(nodeClicked(QModelIndex)));

    setLayout(mainLayout); // レイアウトの指定
}

void TrafficWindow::nodeClicked(QModelIndex index)
{
    // ノードクリック時
    int row = index.row();  // 行
    if (trafficSeriesList[row]->isVisible()) { // 表示されているならば非表示に
        trafficSeriesList[row]->setVisible(false);
        // テーブルの色を消す
        for (int j = 0; j < 3; j++)
            model->setData(model->index(row, j, QModelIndex()),
                           QColor(tr("#ffffff")), Qt::BackgroundColorRole);
    } else { // 非表示ならば表示に
        trafficSeriesList[row]->setVisible(true);
        // テーブルの行に色付け
        QColor rowColor = trafficSeriesList[row]->color();
        for (int j = 0; j < 3; j++)
            model->setData(model->index(row, j, QModelIndex()),
                           rowColor, Qt::BackgroundColorRole);
    }
}

void TrafficWindow::setData(QList<QStringList> &tableData,
                            QMap<int, QList<double_t> > &graphData)
{
    int size = tableData.count();   // ノードの組み数

    // テーブルモデルにデータを追加
    model->insertRows(0, size, QModelIndex());
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 3; j++) {
            // 送信元,宛先,平均トラフィック量
            model->setData(model->index(i, j, QModelIndex()),
                           tableData[i][j]);
        }
    }

    // トラフィック量折れ線データを生成
    for (int key = 0; key < size; key++) {
        QList<double_t> dataList = graphData[key];
        QLineSeries *series = new QLineSeries;  // シリーズの生成
        int size = dataList.count();
        for(int i = 0; i < size; i++){
            series->append(i * interval, dataList[i]); // シリーズにデータを追加
        }
        // キャプション名
        QString name = QString::number(key + 1);
        series->setName(name);  // シリーズの名前を設定
        trafficSeriesList.insert(key, series);  // シリーズリストへの追加
        chart->addSeries(trafficSeriesList[key]);   // チャートへの追加
        trafficSeriesList[key]->setVisible(false);  // シリーズを非表示に
    }
    trafficSeriesList[0]->setVisible(true); // 全体のシリーズのみ表示
    QColor rowColor = trafficSeriesList[0]->color();
    for (int j = 0; j < 3; j++)
        model->setData(model->index(0, j, QModelIndex()),
                       rowColor, Qt::BackgroundColorRole); // テーブルに色付け
    chart->createDefaultAxes(); // 軸を表示
    chart->legend();
    chart->legend()->setAlignment(Qt::AlignBottom);
}
