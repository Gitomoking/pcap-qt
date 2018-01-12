#include <QtWidgets>
#include <QGridLayout>
#include<iostream>
#include "pieview.h"
#include "protocolwindow.h"

ProtocolWindow::ProtocolWindow()
{
    setupModel();   // モデルの作成
    setupViews();   // ビューの作成

    setWindowTitle(tr("プロトコル"));    // ウィンドウタイトル
    resize(800, 650);  // ウィンドウサイズ
}

void ProtocolWindow::setTitle(QString &title)
{
    setWindowTitle(title);
}

void ProtocolWindow::setupModel()
{
    traModel = new QStandardItemModel(4, 3, this);  // ネットワーク／トランスポートモデル
    appModel = new QStandardItemModel(9, 3, this);  // アプリケーションモデル

    // トランスポートモデルのヘッダ設定
    traModel->setHeaderData(0, Qt::Horizontal, tr("プロトコル"));
    traModel->setHeaderData(1, Qt::Horizontal, tr("パケット数"));
    traModel->setHeaderData(2, Qt::Horizontal, tr("存在比"));

    // アプリケーションモデルのヘッダ設定
    appModel->setHeaderData(0, Qt::Horizontal, tr("プロトコル"));
    appModel->setHeaderData(1, Qt::Horizontal, tr("パケット数"));
    appModel->setHeaderData(2, Qt::Horizontal, tr("存在比"));
}

void ProtocolWindow::setData(QList<QStringList> &data)
{
    // トランスポートモデルのデータ設定
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 3; j++){
            traModel->setData(traModel->index(i, j, QModelIndex()), data[i][j]);
        }
        traModel->setData(traModel->index(i, 0, QModelIndex()),
                          QColor(data[i][3]), Qt::DecorationRole);
    }

    // アプリケーションモデルのデータ設定
    for(int i = 4; i < data.count(); i++){
        for(int j = 0; j < 3; j++){
            appModel->setData(appModel->index(i - 4, j, QModelIndex()), data[i][j]);
        }
        appModel->setData(appModel->index(i - 4, 0, QModelIndex()),
                          QColor(data[i][3]), Qt::DecorationRole);
    }
}

void ProtocolWindow::setupViews()
{
    QGridLayout *mainLayout = new QGridLayout;  // レイアウト

    QTableView *traTable = new QTableView;  // ネットワーク／トランスポートテーブル
    QTableView *appTable = new QTableView;  // アプリケーションテーブル
    PieView *traView = new PieView; // ネットワーク／トランスポート円グラフ
    PieView *appView = new PieView; // アプリケーション円グラフ

    // レイアウトの設定
    mainLayout->addWidget(traTable, 0, 0);
    mainLayout->addWidget(appTable, 1, 0);
    mainLayout->addWidget(traView, 0, 1);
    mainLayout->addWidget(appView, 1, 1);

    // ビューへのモデルの指定
    traTable->setModel(traModel);
    traView->setModel(traModel);
    appTable->setModel(appModel);
    appView->setModel(appModel);

    // ヘッダビューの設定
    QHeaderView *traHeaderView = traTable->horizontalHeader();
    traHeaderView->setStretchLastSection(true);

    QHeaderView *appHeaderView = appTable->horizontalHeader();
    appHeaderView->setStretchLastSection(true);

    traTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    appTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // テーブルと円グラフを編集不可にする
    traTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    appTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    traView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    appView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setLayout(mainLayout);  // レイアウト指定
}
