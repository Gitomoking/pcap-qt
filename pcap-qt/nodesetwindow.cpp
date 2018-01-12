#include "nodesetwindow.h"
#include <QTableView>
#include <QHBoxLayout>
#include <QtCharts/QBarCategoryAxis>

using namespace QtCharts;

NodeSetWindow::NodeSetWindow()
{
    setupModel();   // モデルのセットアップ
    setupViews();   // ビューのセットアップ

    setWindowTitle(tr("トラフィック量"));  // ウィンドウタイトル
    resize(1000, 500);  // ウィンドウサイズ
}

void NodeSetWindow::setTitle(QString &title)
{
    setWindowTitle(title);
}

void NodeSetWindow::setupModel()
{
    // テーブルモデルの生成
    model = new QStandardItemModel(0,3, this);
    model->setHeaderData(0, Qt::Horizontal, tr("送信元"));
    model->setHeaderData(1, Qt::Horizontal, tr("宛先"));
    model->setHeaderData(2, Qt::Horizontal, tr("トラフィック量"));
}

void NodeSetWindow::setupViews()
{
    QTableView *table = new QTableView; // テーブルビューの生成
    table->setModel(model); // テーブルビューへのモデルの指定

    // グラフを生成
    chart = new QChart();
    chart->setTitle(tr("通信量の多いノードの組み"));

    // 棒グラフのビューを生成
    barChart = new QChartView();
    barChart->setChart(chart);
    barChart->setRenderHint(QPainter::Antialiasing);

    // レイアウトの設定
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(table);
    mainLayout->addWidget(barChart);

    // テーブルを編集不可にする
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // セルクリック時に行を選択するように設定
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    setLayout(mainLayout);  // レイアウトの指定
}

void NodeSetWindow::setData(QList<QStringList> &data)
{
    int size = data.count();    // ノードの組み数

    // テーブルモデルにデータを追加
    model->insertRows(0, size, QModelIndex());
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 3; j++) {
            // 送信元,宛先,平均トラフィック量
            model->setData(model->index(i, j, QModelIndex()),
                           data[i][j]);
        }
    }

    QList<QBarSet*> barList;    // 棒グラフのデータ生成
    QBarSet *set = new QBarSet(tr("NodeSet"));   // 棒生成
    QStringList names;  // 軸ラベル
    // 棒グラフにデータを追加
    for (int i = 1; i < 6; i++) {
        QString name;
        name = tr("行番号") + QString::number(i + 1);
        names << name;  // 軸ラベル追加
        *set << data[i][2].toDouble();  // 棒のデータ
    }
    barList.append(set);

    QBarSeries *series = new QBarSeries();  // 棒グラフ生成
    series->append(barList);    // 棒グラフに追加

    chart->addSeries(series);   // ビューに棒グラフを指定
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(names);    // 軸ラベル設定
    chart->createDefaultAxes(); // 軸を表示
    chart->setAxisX(axis, series);  // x軸を設定
    chart->legend()->hide();  // キャプションを非表示

    // グラフ表示の部分を色付け
    QColor setColor = set->color();
    for (int i = 1; i < 6; i++) {
        for (int j = 0; j < 3; j++) {   // 選択範囲に色付け
            model->setData(model->index(i, j, QModelIndex()),
                           setColor, Qt::BackgroundColorRole);
        }
    }
}
