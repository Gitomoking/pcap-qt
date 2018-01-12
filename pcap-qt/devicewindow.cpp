#include "devicewindow.h"
#include <QTableView>
#include <iostream>

DeviceWindow::DeviceWindow()
{
    current = 0;

    // モデルの作成
    model = new QStandardItemModel(0, 4);
    model->setHeaderData(0, Qt::Horizontal, tr("デバイス名"));
    model->setHeaderData(1, Qt::Horizontal, tr("説明"));
    model->setHeaderData(2, Qt::Horizontal, tr("アドレス"));
    model->setHeaderData(3, Qt::Horizontal, tr("ネットマスク"));

    // 各ウィジットの作成
    QTableView *table = new QTableView;
    cancelButton = new QPushButton(tr("&キャンセル"));
    okButton = new QPushButton(tr("&確定"));
    okButton->setDisabled(true);

    table->setModel(model); // モデルを指定
    // テーブルを編集不可に，また行選択にする
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    // シグナルとスロットの接続
    connect(table, SIGNAL(clicked(QModelIndex)),
            this, SLOT(deviceClicked(QModelIndex)));
    connect(table, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(deviceDoubleCliecked(QModelIndex)));
    connect(cancelButton, SIGNAL(clicked(bool)),
            this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked(bool)),
            this, SLOT(accept()));

    // 列幅の設定
    table->setColumnWidth(0, 100);
    table->setColumnWidth(1, 200);
    table->setColumnWidth(2, 100);
    table->setColumnWidth(3, 100);

    // テーブルのヘッダ設定
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->hide();

    // レイアウト設定
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(table);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);  // レイアウト指定

    resize(540, 300);
    setWindowTitle(tr("デバイス一覧"));
}

void DeviceWindow::setDeviceData(QList<QStringList> &data)
{
    deviceList = data;  // データを代入

    // モデルにデータを挿入
    int size = deviceList.count();
    model->insertRows(0, size, QModelIndex());
    for (int i = 0; i < size; i++) {
        // wl, eth, or en で始まるデバイスであり
        if (deviceList[i][0].contains("wl") ||
            deviceList[i][0].contains("eth") ||
            deviceList[i][0].contains("en")) {
            // ネットワークアドレスとサブネットマスクが存在する場合
            if (deviceList[i][2].count() && deviceList[i][3].count())
                deviceList[i].append(tr("1")); // 適している場合は1を追加
        }
        if (deviceList[i].count() == 4)
            deviceList[i].append(tr("0"));
        for (int j = 0; j < 4; j++) {
            model->setData(model->index(i, j, QModelIndex()),
                           deviceList[i][j]);
            if (deviceList[i][4] == "1")   // 背景色を指定
                model->setData(model->index(i, j, QModelIndex()),
                               QColor("#9ca7e2"), Qt::BackgroundColorRole);
        }
    }
}

void DeviceWindow::deviceClicked(QModelIndex index)
{
    current = index.row();
    if (deviceList[current][4] == "1")  // 選択できるデバイス
        okButton->setEnabled(true); // 確定ボタンを押せるようにする
    else    // 選択できない
        okButton->setDisabled(true);
}

void DeviceWindow::deviceDoubleCliecked(QModelIndex index)
{
    current = index.row();
    if (deviceList[current][4] == "1")  // 選択できるデバイス
        accept();
}
