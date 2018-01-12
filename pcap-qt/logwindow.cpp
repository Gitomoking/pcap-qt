#include "logwindow.h"
#include "protocolwindow.h"
#include "trafficwindow.h"
#include "nodesetwindow.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <iostream>

LogWindow::LogWindow()
{
    createActions();    // アクションの作成

    // モデルの作成
    model = new QStandardItemModel(0, 2);
    model->setHeaderData(0, Qt::Horizontal, tr("名前"));
    model->setHeaderData(1, Qt::Horizontal, tr("取得日時"));

    // ウィジットの作成
    QTableView *table = new QTableView;
    updateButton = new QPushButton(tr("更新する"));
    closeButton = new QPushButton(tr("閉じる"));

    updateButton->setDisabled(true);

    // テーブルの設定
    table->setModel(model);
    table->setColumnWidth(0, 300);
    table->horizontalHeader()->setStretchLastSection(1);
    table->verticalHeader()->hide();
    table->setEditTriggers(QAbstractItemView::DoubleClicked);   // ダブルクリックで編集可能にする
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    // レイアウトの設定
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(table);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // シグナルとスロットの設定
    connect(table, SIGNAL(clicked(QModelIndex)),
            this, SLOT(dataClicked(QModelIndex)));
    connect(updateButton, SIGNAL(clicked(bool)),
            this, SLOT(updateDatabase()));
    connect(closeButton, SIGNAL(clicked(bool)),
            this, SLOT(hideEvent()));

    // ウィンドウサイズとタイトルの設定
    resize(500,500);
    setWindowTitle(tr("ログ一覧"));
}

void LogWindow::setDBaccess(DBAccess *dbaccess)
{
    DB = dbaccess;
}

void LogWindow::initialize()
{
    loadData(); // ログデータを読み出す

    updateButton->setDisabled(true);

    // データが編集されたときにdataUpdated()を実行するように設定
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(dataUpdated()));
}

void LogWindow::hideEvent()
{
    if (updateMap.count())
        updateDatabase();   // データを更新

    // データ編集時のシグナルとスロットの接続を削除
    disconnect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
               this, SLOT(dataUpdated()));
    model->removeRows(0, model->rowCount(), QModelIndex());   // 行削除
    logList.clear();    // ログ一覧削除
    currentRow = -1;
    hide();
}

#ifndef QT_NO_CONTEXTMENU
void LogWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if (currentRow == -1) return;
    QMenu menu(this);   // コンテキストメニュー
    // アクションの追加
    menu.addAction(protoAction);
    menu.addAction(trafficAction);
    menu.addAction(nodesAction);
    menu.addAction(flagAction);
    menu.addAction(deleteAction);
    // コンテキストメニューの実行
    menu.exec(event->globalPos());
}
#endif

void LogWindow::dataUpdated()
{
    if (currentRow == -1) return;
    // 編集されたデータをリストに格納する
    QStringList data;
    if (updateMap.contains(currentRow))
        data = updateMap[currentRow];
    else
        data = logList[currentRow];

    // 更新されたログの説明を格納
    data[3] = model->index(currentRow, 0, QModelIndex()).data().toString();
    updateMap[currentRow] = data;   // 更新リストに格納
    updateButton->setEnabled(true); // 更新ボタンクリックを可能にする
}

void LogWindow::changeFlag()
{
    if (currentRow == -1) return;
    QStringList data;
    if (updateMap.contains(currentRow))
        data = updateMap[currentRow];
    else
        data = logList[currentRow];

    int temp = currentRow;
    currentRow = -1;
    // フラグの色を変える
    if (data[2] == "1") {
        data[2] = "0";
        for (int j = 0; j < 2; j++)
            model->setData(model->index(temp, j, QModelIndex()),
                           QColor(Qt::white), Qt::BackgroundColorRole);
    } else {
        data[2] = "1";
        for (int j = 0; j < 2; j++)
            model->setData(model->index(temp, j, QModelIndex()),
                           QColor(flagColor), Qt::BackgroundColorRole);
    }
    currentRow = temp;
    updateMap[currentRow] = data;
    updateButton->setEnabled(true); // 更新ボタンクリックを可能にする
}

void LogWindow::updateDatabase()
{
    // 更新されたすべてのログに対して
    foreach (const QStringList log, updateMap) {
        int logid = log[0].toInt();    // ログID
        int flag = log[2].toInt(); // フラグ
        QString memo = log[3]; // ログの説明
        DB->updateLogData(logid, flag, memo);    // データベースを更新
    }
    updateMap.clear();  // 更新リストをクリア
    updateButton->setDisabled(true);    // 更新ボタンクリックを不可にする
}

void LogWindow::dataClicked(QModelIndex index)
{
    // データがクリックされたとき，その行を格納
    currentRow = index.row();
}

void LogWindow::deleteLog()
{
    if (currentRow == -1) return;
    // currentRowで表されるデータを削除
    int logid = logList[currentRow][0].toInt();    // ログID
    DB->deleteLogData(logid);    // データベースから削除
    model->removeRow(currentRow, QModelIndex());  // モデルから削除
    logList.removeAt(currentRow);   // ログリストから削除
    if (updateMap.contains(logid))
        updateMap.remove(logid);    // 更新リストから削除
}

void LogWindow::showProtocolWindow()
{
    if (currentRow == -1) return;
    int logid = logList[currentRow][0].toInt(); // ログID
    if (!protoMap.contains(currentRow)) {
        protoMap[currentRow] = DB->selectProtocol(logid);   // 取得
    }

    // プロトコル統計ウィンドウ生成
    ProtocolWindow *PWindow = new ProtocolWindow();
    PWindow->setData(protoMap[currentRow]); // データの設定
    QString title = logList[currentRow][3]; // ログ名
    PWindow->setTitle(title);
    PWindow->show();    // ウィンドウの表示
}

void LogWindow::showTrafficWindow()
{
    if (currentRow == -1) return;
    // ノードの組み統計データ取得
    int logid = logList[currentRow][0].toInt(); // ログID
    nodesMap[currentRow] = DB->selectNodeSet(logid);

    QList<int> nodeIDList;

    // ノードの組みIDをリストにする
    int size = nodesMap[currentRow].count();
    for (int i = 0; i < size; i++) {
        // ノードの組みIDリストを作成し，nodesMapのそれぞれのノードの組みデータから
        // 先頭データのIDを削除する
        nodeIDList.append(nodesMap[currentRow][i][0].toInt()); // ノードの組みID追加
        nodesMap[currentRow][i].removeFirst(); // ノードの組みID削除
    }

    if (!trafficMap.contains(currentRow)) {   // トラフィック量統計データ取得
        int logid = logList[currentRow][0].toInt(); // ログID
        trafficMap[currentRow] = DB->selectTraffic(logid, nodeIDList);
    }

    // トラフィック量統計ウィンドウ生成
    TrafficWindow *TWindow = new TrafficWindow();
    TWindow->setData(nodesMap[currentRow], trafficMap[currentRow]);    // データの設定
    QString title = logList[currentRow][3]; // ログ名
    TWindow->setTitle(title);
    TWindow->show();    // ウィンドウの表示
}

void LogWindow::showNodeSetWindow()
{
    if (currentRow == -1) return;
    int logid = logList[currentRow][0].toInt(); // ログID
    if (!nodesMap.contains(currentRow)) {
        nodesMap[currentRow] = DB->selectNodeSet(logid);    // 取得
        // 先頭のノードの組みIDを削除
        int size = nodesMap[currentRow].count();
        for (int i = 0; i < size; i++) {
            nodesMap[currentRow][i].removeFirst();
        }
    }

    // ノードの組み統計ウィンドウ生成
    NodeSetWindow *NWindow = new NodeSetWindow();
    NWindow->setData(nodesMap[currentRow]);
    QString title = logList[currentRow][3]; // ログ名
    NWindow->setTitle(title);
    NWindow->show();
}

void LogWindow::loadData()
{
    // データベースからログデータを読み出す
    logList = DB->selectLogData();
    int size = logList.count();
    for (int i = 0; i < size; i++) {
        // log のデータ形式
        // ログID,取得時刻,フラグ,ログの説明
        QStringList log = logList[i];   // ログデータ
        model->insertRow(i, QModelIndex());
        model->setData(model->index(i, 0, QModelIndex()),
                       log[3]); // ログの名前
        model->setData(model->index(i, 1, QModelIndex()),
                       log[1]); // 取得時刻
        if (log[2] == "1") {
            // フラグが立っている場合は背景色を指定
            for (int j = 0; j < 2; j++)
                model->setData(model->index(i, j, QModelIndex()),
                               QColor(flagColor), Qt::BackgroundColorRole);
        }
    }
}

void LogWindow::createActions()
{
    protoAction = new QAction(tr("プロトコル統計"));
    connect(protoAction, SIGNAL(triggered(bool)),
            this, SLOT(showProtocolWindow()));
    trafficAction = new QAction(tr("トラフィック量統計"));
    connect(trafficAction, SIGNAL(triggered(bool)),
            this, SLOT(showTrafficWindow()));
    nodesAction = new QAction(tr("ノードの組み統計"));
    connect(nodesAction, SIGNAL(triggered(bool)),
            this, SLOT(showNodeSetWindow()));
    flagAction = new QAction(tr("フラグの切り替え"));
    connect(flagAction, SIGNAL(triggered(bool)),
            this, SLOT(changeFlag()));
    deleteAction = new QAction(tr("削除"));
    connect(deleteAction, SIGNAL(triggered(bool)),
            this, SLOT(deleteLog()));
}
