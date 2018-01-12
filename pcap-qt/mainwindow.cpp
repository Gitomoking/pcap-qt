#include "mainwindow.h"
#include "devicewindow.h"
#include "filterdialog.h"
#include "filterexpressionwindow.h"
#include "protocolwindow.h"
#include "trafficwindow.h"
#include "nodesetwindow.h"
#include "logsavedialog.h"
#include <QtWidgets>
#include <iostream>
#include <QtAlgorithms>

const QString pcap_device("../pcap/pcap-device");
const QString pcap_capture("../pcap/pcap-capture");
const QString pseudo_capture("../pseudo-pcap/pseudo-capture");

MainWindow::MainWindow()
{
    maxrow = 0;     // モデルのデータ数
    captureCount = 0;   // キャプチャを実行した回数
    colorable = true;   // パケット色分けを行う
    visualable = true;  // 可視化を行う
    running = false;    // キャプチャ開始前
    autoscrollable = true;  // 自動スクロールを行う
    saved = true;       // ログ保存初期値
    searching = false;  // 検索前
    nowRowIndex = -1;   // 検索結果で表示している行
    deleteFlag = false; // 行削除かどうか

    createActions();    // アクションの作成
    createMenus();      // メニューバーの作成
    createToolBar();    // ツールバーの作成

    DB = new DBAccess;
    if (!DB->createConnection()) {
        QString text("データベースに接続できません\nログ機能を利用できません");
        showWarning(text);
        dbconnection = false;
        logAction->setDisabled(true);
    } else dbconnection = true;

    setupModel();       // モデルの設定
    setupView();        // ビューの設定

    setWindowIcon(QIcon(":/images/fasticon-green-ville-network.ico"));
    setWindowTitle(tr("パケットキャプチャ"));   // ウィンドウタイトル
    resize(800,600);    // ウィンドウ初期サイズ

    VWindow = new VisualizationWindow();
}

void MainWindow::showVWindow()
{
    VWindow->show();
}

void MainWindow::start()
{
    // デバイスリストを取得
    process = new QProcess(this);
    process->start(pcap_device); // デバイス一覧取得プログラムの実行
    // 子プロセスからの標準出力を読み込むように設定
    connect(process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readData()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(processError(QProcess::ProcessError)));
    process->waitForFinished();  // 子プロセス終了待ち
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // ウィンドウが閉じられるときに実行される処理
    stopPacketCapture();    // パケットキャプチャ終了

    // ログ保存済みかどうか
    if (!saved) {
        if (!showLogSaveWarning()) { // ログ保存警告メッセージを表示
             // ログ保存キャンセル
            event->ignore();    // ウィンドウを閉じない
            return;
        }
    }

    DB->closeConnection();  // データベースコネクション
    if (LWindow) delete LWindow;
    VWindow->close();   // 可視化ウィンドウを閉じる
    event->accept();    // ウィンドウを閉じる
}

void MainWindow::setCaptureDevice(int index)
{
    device = deviceList[index][0]; // 現在のデバイス変更
    startPacketCapture();   // パケットキャプチャ開始
}

void MainWindow::switchVWindow()
{
    // 可視化ウィンドウの表示切り替えを行う
    if (visualable) {   // 表示中の場合
        visualable = false; // 非表示にする
        VWindow->clearWindow();
        VWindow->hide();
    } else {    // 非表示中の場合
        visualable = true;  // 表示する
        VWindow->show();
    }
}

void MainWindow::setToolBarVisible()
{
    // ツールバーの表示または非表示
    if (mainToolBar->isVisible())
        mainToolBar->setVisible(false);
    else
        mainToolBar->setVisible(true);
}

void MainWindow::setPacketColorVisible()
{
    if (colorable)  // パケットの色分け設定変更
        colorable = false;
    else
        colorable = true;
}

void MainWindow::setDetailVisible()
{
    // パケット詳細の表示／非表示変更
    if (detailWidget->isVisible())
        detailWidget->hide();
    else detailWidget->show();
}

void MainWindow::setRowDataVisible()
{
    // 生パケットデータの表示／非表示変更
    if (rowDataWidget->isVisible())
        rowDataWidget->hide();
    else rowDataWidget->show();
}

void MainWindow::scrollToTop()
{
    table->scrollToTop();   // 先頭パケットへスクロール
    if (maxrow) {
        // 先頭パケットを選択状態にする
        QModelIndex index = model->index(0, 0);
        selectionModel->select(index, QItemSelectionModel::Rows |
                               QItemSelectionModel::ClearAndSelect);
        dispDetail(0);  // パケット詳細を表示
    }
}

void MainWindow::scrollToBottom()
{
    table->scrollToBottom();    // 末尾パケットへスクロール
    if (maxrow) {
        // 末尾パケットを選択状態にする
        QModelIndex index = model->index(maxrow - 1, 0);
        selectionModel->select(index, QItemSelectionModel::Rows |
                               QItemSelectionModel::ClearAndSelect);
        dispDetail(maxrow -1);  // パケット詳細を表示
    }
}

void MainWindow::scrollToPrevious()
{
    // 前検索結果データにスクロールする
    if (nowRowIndex > 0) {
        nowRowIndex--;
        table->scrollTo(model->index(searchResult[nowRowIndex], 0));
        dispDetail(-1);
    } else {    // 先頭データの場合
        nowRowIndex = searchResult.count() - 1;
        table->scrollTo(model->index(searchResult[nowRowIndex], 0));
        dispDetail(-1);
    }
    QModelIndex index = model->index(searchResult[nowRowIndex], 0, QModelIndex());
    selectionModel->select(index, QItemSelectionModel::Rows |
                           QItemSelectionModel::ClearAndSelect);
}

void MainWindow::scrollToNext()
{
    // 次検索結果データにスクロールする
    if (nowRowIndex < searchResult.count() - 1) {
        nowRowIndex++;
        table->scrollTo(model->index(searchResult[nowRowIndex], 0));
        dispDetail(-1);
    } else {    // 末尾データの場合
        nowRowIndex = 0;
        table->scrollTo(model->index(searchResult[nowRowIndex], 0));
        dispDetail(-1);
    }
    QModelIndex index = model->index(searchResult[nowRowIndex], 0, QModelIndex());
    selectionModel->select(index, QItemSelectionModel::Rows |
                           QItemSelectionModel::ClearAndSelect);
}

void MainWindow::setAutoScroll()
{
    // 自動スクロールのオン／オフ変更
    if (autoscrollable) {
        autoscrollable = false;
        autoScrollAction->setChecked(false);
    } else {
        autoscrollable = true;
        autoScrollAction->setChecked(true);
    }
}

void MainWindow::readData()
{
    QString newReadData = process->readLine();  // 子プロセスからのデータを読み出す
    if (newReadData.size()) {   // データを読み出せた場合
        QStringList newData = newReadData.split("|");   // データをリストに分割
        switch (newData[0].toInt()) {

            case READ_DEVICE : {
                /* デバイスデータの形
                 * 6|デバイス名|デバイス詳細|ネットワークアドレス|ネットマスク */
                newData.removeFirst();
                deviceList.append(newData); // デバイスリストに追加
            } break;

            case READ_MAINPACKET : {
                /* パケットデータの形
                 * 1|データ長 \n
                 * パケットID|エラー情報(なし1 or あり0)|取得時刻|送信元|宛先|パケット長|
                 * プロトコル|備考|データリンク層詳細|ネットワーク層詳細|トランスポート層詳細|
                 * アプリケーション層詳細|生データ */

                // パケットデータを読み込む
                QString newPacketReadData = process->read(newData[1].toInt());

                // それぞれのパケットデータごとに分ける
                QStringList newPacket = newPacketReadData.split("|");
                if (newPacket.count() == 13){
                    if (visualable) {   // 可視化を行う場合
                        QStringList visualData; // 可視化用データ
                        // パケットID,エラー情報,送信元,宛先,パケット長,プロトコル
                        for (int i = 0; i < 7; i++) {
                            if (i == 2) continue;
                            visualData.append(newPacket[i]);
                        }
                        VWindow->addData(visualData);
                    }

                    // エラー有無／プロトコルごとに色を決定
                    if (!QString::compare(newPacket[1], tr("0")))
                        newPacket.append(errorColor);
                    else if (!QString::compare(newPacket[6], tr("ARP")))
                        newPacket.append(arpColor);
                    else if (!QString::compare(newPacket[6], tr("TCP")))
                        newPacket.append(tcpColor);
                    else if (!QString::compare(newPacket[6], tr("UDP")))
                        newPacket.append(udpColor);
                    else if (!QString::compare(newPacket[6], tr("ICMP")))
                        newPacket.append(icmpColor);
                    else if (!QString::compare(newPacket[6], tr("DNS")))
                        newPacket.append(dnsColor);
                    else if (!QString::compare(newPacket[6], tr("HTTP")))
                        newPacket.append(httpColor);
                    else if (!QString::compare(newPacket[6], tr("HTTPS")))
                        newPacket.append(httpsColor);
                    else if (!QString::compare(newPacket[6], tr("FTP")))
                        newPacket.append(ftpColor);
                    else if (!QString::compare(newPacket[6], tr("NTP")))
                        newPacket.append(ntpColor);
                    else if (!QString::compare(newPacket[6], tr("DHCP")))
                        newPacket.append(dhcpColor);
                    else if (!QString::compare(newPacket[6], tr("Telnet")))
                        newPacket.append(telnetColor);
                    else if (!QString::compare(newPacket[6], tr("SSH")))
                        newPacket.append(sshColor);
                    else
                        newPacket.append(otherColor);

                    setPacketData(newPacket);   // パケットデータを追加
                }
            } break;

            case READ_PROTOCOL : {
                /* プロトコル統計データの形
                 * 2|プロトコル名|パケット数|存在比 */
                newData.removeFirst();  // 先頭の識別子を削除
                // プロトコルごとに色を決定
                if (!QString::compare(newData[0], tr("ARP")))
                    newData.append(arpColor);
                if (!QString::compare(newData[0], tr("TCP")))
                    newData.append(tcpColor);
                if (!QString::compare(newData[0], tr("UDP")))
                    newData.append(udpColor);
                if (!QString::compare(newData[0], tr("ICMP")))
                    newData.append(icmpColor);
                if (!QString::compare(newData[0], tr("DNS")))
                    newData.append(dnsColor);
                if (!QString::compare(newData[0], tr("HTTP")))
                    newData.append(httpColor);
                if (!QString::compare(newData[0], tr("HTTPS")))
                    newData.append(httpsColor);
                if (!QString::compare(newData[0], tr("FTP")))
                    newData.append(ftpColor);
                if (!QString::compare(newData[0], tr("NTP")))
                    newData.append(ntpColor);
                if (!QString::compare(newData[0], tr("DHCP")))
                    newData.append(dhcpColor);
                if (!QString::compare(newData[0], tr("Telnet")))
                    newData.append(telnetColor);
                if (!QString::compare(newData[0], tr("SSH")))
                    newData.append(sshColor);
                if (!QString::compare(newData[0], tr("その他")))
                    newData.append(otherColor);
                protocolList.append(newData);   // データの追加
            } break;

            case READ_TRAFFIC : {
                /* トラフィック量統計データの形
                 * 3|送信元|宛先|データ識別子|トラフィック量 */
                newData.removeFirst();  // 先頭の識別子を削除
                QStringList node(QStringList()
                                 << newData[0] << newData[1]);

                if (trafficTemp.contains(node)) {   // ノードが存在する場合
                    trafficTemp[node].append(newData[3].toDouble());
                } else {
                    QList<double_t> trafficList;
                    int num = newData[2].toInt();   // 何番目のデータか
                    for (int i = 0; i < num; i++) {
                        trafficList.append(0.0);    // 0を追加
                    }
                    trafficList.append(newData[3].toDouble()); // トラフィック量追加
                    trafficTemp[node] = trafficList;    // リストの追加
                }
            } break;

            case READ_NODES : {
                /* ノードの組み統計データの形
                 * 4|送信元|宛先|平均トラフィック量 */
                newData.removeFirst();  // 先頭の識別子を削除
                nodesList.append(newData);  // データの追加
            } break;

            case READ_END : {
                // データの終了，ソートをし，アクション可能にする
                sortNodesList();    // nodesListを降順ソート
                int size = nodesList.count();
                // trafficMapにデータを格納
                if (trafficTemp.count()) {
                    for (int i = 0; i < size; i++) {
                        QList<double_t> traffics;
                        QStringList key = QStringList() << nodesList[i][0] << nodesList[i][1];
                        traffics = trafficTemp[key];   // トラフィック量を取得
                        trafficMap[i] = traffics;   // データを格納
                    }
                    seriesAction->setEnabled(true);
                    barAction->setEnabled(true);
                }
                startAction->setEnabled(true);
                circleAction->setEnabled(true);
            } break;
        }
    }
    if (process->canReadLine())  // まだ読み出せる場合
        readData(); // 読み出し
}

void MainWindow::packetSelected(QModelIndex current)
{
    if (deleteFlag) return; // 削除時は実行されない
    // パケットデータが選択されたときに実行
    autoscrollable = false; // 自動スクロールをオフ
    autoScrollAction->setChecked(false);
    int row = current.row();    // 選択された行
    if (row < packetList.count()) {
        dispDetail(row);    // パケット詳細，生データを表示
    }
    if (searching) {    // 検索中の場合
        int index = searchResult.indexOf(row);
        if (index > -1) {   // 検索結果に該当する場合
            search->setCurrentRow(index);   // 現在選択中の行を更新
            nowRowIndex = index;    // 現在選択中の行を更新
        }
    }
}

void MainWindow::startPacketCapture()
{
    if (!device.count()) {   // デバイスが存在しない場合デバイス選択
        showDeviceWindow();
        return;
    }

    if (running) {  // パケットキャプチャ実行中の場合
        stopPacketCapture();    // キャプチャを終了
    }

    if (!saved) {   // ログが保存されていない場合
        if (!showLogSaveWarning())  // ログ保存警告を表示
            return; // キャンセルされた場合，戻る
    }

    // フィルタダイアログを表示
    FilterDialog *filterDialog = new FilterDialog();
    if (filterDialog->exec() == QDialog::Accepted) {

        stopPacketCapture();    // 現在実行中のパケットキャプチャを終了

        QStringList arguments("");  // 子プロセスの引数（フィルタ式）
        filExp = filterDialog->data();  // フィルタ式
        arguments = filExp.split(" ");  // フィルタ式を引数に変換
        arguments.insert(0, device);    // 引数作成

        // 引数をコンソールに表示
        foreach (const QString arg, arguments)
            std::cout<<arg.toStdString()<<" ";
        std::cout<<std::endl;

        // アクションを有効／無効にする
        startAction->setDisabled(true);
        circleAction->setDisabled(true);
        seriesAction->setDisabled(true);
        barAction->setDisabled(true);
        stopAction->setEnabled(true);
        toTopAction->setEnabled(true);
        toBottomAction->setEnabled(true);
        autoScrollAction->setChecked(true);
        autoscrollable = true;

        if (captureCount > 0) {   // 2回目の生成の場合
            detailText->clear(); // パケット詳細をクリア
            rowDataText->clear(); // 生パケットをクリア

            deleteFlag = true;
            model->removeRows(0, packetList.count()); // テーブルのデータをクリア
            deleteFlag = false;

            // データをクリア
            packetList.clear();
            protocolList.clear();
            nodesList.clear();
            trafficMap.clear();
            maxrow = 0; // パケット取得数を0にする

            // 可視化ウィンドウを初期化
            VWindow->clearWindow();
        }

        process = new QProcess(this);   // プロセスの生成
        process->start(pseudo_capture, arguments); // 外部プログラムの実行

        captureCount++; // キャプチャ回数をインクリメント
        running = true; // キャプチャ中状態に移行

        // 子プロセスの標準出力から読み込み可能になったらreadData()を呼び出す
        connect(process, SIGNAL(readyReadStandardOutput()),
                this, SLOT(readData()));
        // プロセスが終了したらprocessFinished()を呼び出す
        connect(process, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(processFinished(int,QProcess::ExitStatus)));
        // プロセスがエラーを生じたらprocessError()を呼び出す
        connect(process, SIGNAL(error(QProcess::ProcessError)),
                this, SLOT(processError(QProcess::ProcessError)));
    }
    if (filterDialog) delete filterDialog;
}

void MainWindow::stopPacketCapture()
{
    if (running) {
        if (process->state() == QProcess::Running) {
            // プロセスが実行中状態の場合
            running = false;    // キャプチャ終了状態に移行
            process->terminate();   // 子プロセスにSIGTERMシグナルを送る
            process->waitForFinished(); // 子プロセスの終了待ち
            if (process) delete process;    // プロセスを削除
        }
        stopAction->setDisabled(true);
        autoScrollAction->setChecked(false);
        if (dbconnection)   // データベース接続がある場合
            saved = false;  // ログ未保存
    }
}

void MainWindow::doSearch()
{
    if (searching) finishSearch();  // 検索結果初期化
    QString data = search->searchData();    // 検索文字列

    // データの下準備
    bool condition[3] = { false };
    /* condition : 初期値はfalse
     * 0 : 数字のみ = true
     * 1 : 先頭文字が数字 = true
     * 2 : 文字列長が6以下 = true */

    // 数字のみかどうか
    bool ok;
    data.toInt(&ok, 10);   // 10進数の整数に変換
    if (ok) // 変換成功 (ok = true) の場合
        condition[0] = true;    // 数字のみと判定

    // 先頭文字が数字かどうか
    QChar first = data.at(0);
    if (first.digitValue() > -1)   // 先頭文字を数値に変換できる場合
        condition[1] = true;   // 先頭文字を数字と判定

    // 文字列のサイズが6以下であるか
    if (data.count() < 7)
        condition[2] = true;    // 文字列のサイズが6以下であると判定

    // すべてのパケットについて
    int size = packetList.count();
    for (int i = 0; i < size; i++) {
        QStringList packet = packetList[i];
        if (packet.count() > 14) continue;
        if (condition[0]) { // 数字のみの場合
            // パケット長の検索 [5]
            if (packet[5].contains(data)) {  // パケット長検索成功
                searchResult.append(i);    // 行番号追加
                continue;   // 次データへ
            }
        }
        if (condition[1]) {  // 先頭が数字の場合
            // 送信元と宛先の検索 [3][4]
            if (packet[3].contains(data) || packet[4].contains(data)) {
                searchResult.append(i);    // 行番号追加
                continue;   // 次データへ
            }
        } else if (condition[2]) {  // 文字列長が6以下の場合
            // プロトコルの検索 [6]
            if (packet[6].contains(data)) {
                searchResult.append(i);    // 行番号追加
                continue;   // 次データへ
            }
        }
        // パケット詳細の検索 [7]-[11]
        for (int j = 7; j < 12; j++) {  // 7から11まで
            if (packet[j].contains(data)) { // 検索成功の場合
                searchResult.append(i);    // 行番号追加
                break;  // 次データへ
            }
        }
    }

    // 検索結果（行）を表示
    search->setSearchResult(searchResult.count());

    if (searchResult.count()) { // 検索結果がある場合
        nowRowIndex = 0;    // 先頭の結果を選択
        searching = true;   // 検索中フラグオン
    } else {    // ない場合は終了
        return;
    }

    // 自動スクロールをオフにする
    autoscrollable = false;
    autoScrollAction->setChecked(false);

    // 行に色付けをする
    dispResultRows();

    // 検索結果の先頭にスクロール
    table->scrollTo(model->index(searchResult[0], 0));
    QModelIndex index = model->index(searchResult[0], 0);
    selectionModel->select(index, QItemSelectionModel::Rows |
                           QItemSelectionModel::ClearAndSelect);
    dispDetail(-1); // 詳細表示
}

void MainWindow::finishSearch()
{
    // 検索前の状態に戻す
    nowRowIndex = 0;    // 現在の行
    // 背景色を元に戻す
    while (searchResult.count()) {
        int row = searchResult.first();
        QStringList packet = packetList[row];
        for (int j = 0; j < 7; j++) {
            if (colorable && packet.count() == 14)
                model->setData(model->index(row, j, QModelIndex()),
                               QColor(packet[13]),
                               Qt::BackgroundColorRole);
            else
                model->setData(model->index(row, j, QModelIndex()),
                               QColor(Qt::white),
                               Qt::BackgroundColorRole);
        }
        searchResult.removeFirst();   // 検索リストから削除
    }
    searching = false;  // 検索終了
}

void MainWindow::showDeviceWindow()
{
    // デバイス選択ウィンドウ生成
    DWindow = new DeviceWindow;
    DWindow->setDeviceData(deviceList); // データを渡す
    if (DWindow->exec() == QDialog::Accepted) { // デバイス確定
        int index = DWindow->deviceIndex(); // デバイスリストの添え字
        device = deviceList[index][0];  // デバイス名
        if (DWindow) delete DWindow;
        startPacketCapture();   // パケットキャプチャを開始
    } else {    // キャンセルされた場合
        if (DWindow) delete DWindow;
    }
}

void MainWindow::showFilterWindow()
{
    QString data = filExp.replace(tr("("), tr(""));
    data.replace(tr(")"), tr(""));  // フィルタ式
    // フィルタ式確認ウィンドウを生成
    FilterExpressionWindow *FEWindow = new FilterExpressionWindow(data);
    FEWindow->show();   // ウィンドウを表示
    connect(FEWindow, SIGNAL(changeFilter()), this, SLOT(startPacketCapture()));
}

void MainWindow::showSearchDialog()
{
    // 検索ダイアログを表示
    if (!searching) {
        search->show(); // 検索ダイアログを表示
    }
}

void MainWindow::showProtocolWindow()
{
    // プロトコル統計ウィンドウ生成
    ProtocolWindow *PWindow = new ProtocolWindow();
    PWindow->setData(protocolList); // データの設定
    PWindow->show();    // ウィンドウの表示
}

void MainWindow::showTrafficWindow()
{
    // トラフィック量統計ウィンドウ生成
    TrafficWindow *TWindow = new TrafficWindow();
    TWindow->setData(nodesList, trafficMap);    // データの設定
    TWindow->show();    // ウィンドウの表示
}

void MainWindow::showNodeSetWindow()
{
    // ノードの組み統計ウィンドウ生成
    NodeSetWindow *NWindow = new NodeSetWindow();
    NWindow->setData(nodesList);
    NWindow->show();
}

void MainWindow::showLogWindow()
{
    if (!LWindow->isVisible()) {
        LWindow->initialize();  // ログウィンドウ初期化
        LWindow->show();    // ログウィンドウを表示
    }
}

void MainWindow::showHelp()
{
    // ヘルプを表示
    QMessageBox::aboutQt(this, tr("Qtについて"));
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        QString msg("子プロセスがクラッシュしました");
        showWarning(msg);
        running = false;
        if (process) delete process;
        stopAction->setDisabled(true);
        if (device.count())
            startAction->setEnabled(true);
    } else if (exitCode != 0) {
        QString msg("子プロセスがエラーにより終了しました");
        showWarning(msg);
        running = false;
        if (process) delete process;
        stopAction->setDisabled(true);
        if (device.count())
            startAction->setEnabled(true);
    } else {
        running = false;
        if (!device.count()) { // デバイスが存在しない場合
            if (process) delete process;    // デバイスリスト取得プロセスを削除
            showDeviceWindow(); // デバイス選択ウィンドウを表示
        }
    }
}

void MainWindow::processError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart) {
        QString msg("プログラムの起動エラーです");
        showWarning(msg);
        running = false;
        stopAction->setDisabled(true);
        if (device.count())
            startAction->setEnabled(true);
    }
}

void MainWindow::createActions()
{
    // 「表示」アクション
    // 可視化ウィンドウ表示アクションの作成
    visualAction = new QAction(tr("&可視化ウィンドウ"), this);
    visualAction->setCheckable(true);
    visualAction->setChecked(true);
    connect(visualAction, SIGNAL(triggered(bool)),
            this, SLOT(switchVWindow()));

    // ツールバー表示／非表示アクションの作成
    toolBarAction = new QAction(tr("&ツールバー"), this);
    toolBarAction->setStatusTip(tr("ツールバーの表示／非表示を設定します"));
    toolBarAction->setCheckable(true);
    toolBarAction->setChecked(true);
    connect(toolBarAction, SIGNAL(triggered(bool)),
            this, SLOT(setToolBarVisible()));

    // パケットの色分け設定アクションの作成
    colorAction = new QAction(tr("&パケットの色分け"), this);
    colorAction->setStatusTip(tr("パケットの色分けを設定します"));
    colorAction->setCheckable(true);
    if (colorable) colorAction->setChecked(true);
    connect(colorAction, SIGNAL(triggered(bool)),
            this, SLOT(setPacketColorVisible()));

    // パケット詳細ウィジットの表示アクションの作成
    detailAction = new QAction(tr("&パケット詳細"), this);
    detailAction->setStatusTip(tr("パケット詳細の表示を設定します"));
    detailAction->setCheckable(true);
    detailAction->setChecked(true);
    connect(detailAction, SIGNAL(triggered(bool)),
            this, SLOT(setDetailVisible()));

    // 生パケットデータウィジットの表示アクションの作成
    rowDataAction = new QAction(tr("&生パケットデータ"), this);
    rowDataAction->setStatusTip(tr("生パケットデータの表示を設定します"));
    rowDataAction->setCheckable(true);
    rowDataAction->setChecked(true);
    connect(rowDataAction, SIGNAL(triggered(bool)),
            this, SLOT(setRowDataVisible()));

    // 「キャプチャ」アクション
    // 開始アクションの作成，シグナルとの接続はパケットキャプチャ終了後に行う
    startAction = new QAction(tr("&キャプチャ開始"), this);
    startAction->setIcon(QIcon(":/images/start.png"));
    startAction->setStatusTip(tr("パケットキャプチャを開始します"));
    connect(startAction, SIGNAL(triggered()),
            this, SLOT(startPacketCapture()));

    // 停止アクションの作成
    stopAction = new QAction(tr("&キャプチャ停止"), this);
    stopAction->setIcon(QIcon(":/images/stop.png"));
    stopAction->setStatusTip(tr("パケットキャプチャを停止します"));
    // クリックされたときにパケットキャプチャを停止するよう設定
    connect(stopAction, SIGNAL(triggered()),
            this, SLOT(stopPacketCapture()));
    stopAction->setDisabled(true);

    // デバイス一覧アクション
    deviceAction = new QAction(tr("&デバイス一覧"), this);
    deviceAction->setStatusTip(tr("デバイス一覧を表示します"));
    connect(deviceAction, SIGNAL(triggered(bool)),
            this, SLOT(showDeviceWindow()));

    // フィルタ式確認アクションの作成
    filterAction = new QAction(tr("&フィルタ設定"), this);
    connect(filterAction, SIGNAL(triggered()),
            this, SLOT(showFilterWindow()));

    // スクロールツートップアクションの作成
    toTopAction = new QAction(tr("&最初のパケット"));
    toTopAction->setIcon(QIcon(":/images/up.png"));
    connect(toTopAction, SIGNAL(triggered(bool)),
            this, SLOT(scrollToTop()));
    toTopAction->setDisabled(true);

    // スクロールツーボトムアクションの作成
    toBottomAction = new QAction(tr("&最後のパケット"));
    toBottomAction->setIcon(QIcon(":/images/down.png"));
    connect(toBottomAction, SIGNAL(triggered(bool)),
            this, SLOT(scrollToBottom()));
    toBottomAction->setDisabled(true);

    // 自動スクロールアクションの作成
    autoScrollAction = new QAction(tr("&自動スクロール"), this);
    autoScrollAction->setStatusTip(tr("自動スクロールのON／OFFを設定します"));
    autoScrollAction->setIcon(QIcon(":/images/autoscroll.png"));
    connect(autoScrollAction, SIGNAL(triggered(bool)),
            this, SLOT(setAutoScroll()));
    autoScrollAction->setCheckable(true);
    autoScrollAction->setChecked(autoscrollable);

    // 検索アクションの作成
    searchAction = new QAction(tr("&検索"), this);
    searchAction->setStatusTip(tr("検索ダイアログを開きます"));
    searchAction->setIcon(QIcon(":/images/search.png"));
    connect(searchAction, SIGNAL(triggered(bool)),
            this, SLOT(showSearchDialog()));

    // 「分析」アクション
    // ログアクションの作成
    logAction = new QAction(tr("&ログ"), this);
    logAction->setIcon(QIcon(":/images/database.png"));
    logAction->setStatusTip(tr("過去の統計データ一覧を開きます"));
    // クリックされたときにログ一覧を表示するよう設定
    connect(logAction, SIGNAL(triggered(bool)),
            this, SLOT(showLogWindow()));

    // グラフアクションの作成，シグナルとの接続はパケットキャプチャ終了後
    circleAction = new QAction(tr("&プロトコル統計"), this);
    circleAction->setIcon(QIcon(":/images/circle.png"));
    circleAction->setStatusTip(tr("プロトコルに関する統計データのウィンドウを表示します"));
    circleAction->setDisabled(true);    // デフォルトはクリックできない
    // クリックされたときにプロトコル統計ウィンドウを表示するよう設定
    connect(circleAction, SIGNAL(triggered(bool)),
            this, SLOT(showProtocolWindow()));

    seriesAction = new QAction(tr("&トラフィック量統計"), this);
    seriesAction->setIcon(QIcon(":/images/graph.png"));
    seriesAction->setStatusTip(tr("トラフィック量に関する時間別統計データのウィンドウを表示します"));
    seriesAction->setDisabled(true);
    // クリックされたときにトラフィック量統計ウィンドウを表示するよう設定
    connect(seriesAction, SIGNAL(triggered(bool)),
            this, SLOT(showTrafficWindow()));

    barAction = new QAction(tr("&ネットワークホスト統計"), this);
    barAction->setIcon(QIcon(":/images/BarBar.png"));
    barAction->setStatusTip(tr("通信量に関する統計データのウィンドウを表示します"));
    barAction->setDisabled(true);
    // クリックされたときにノードの組み統計ウィンドウを表示するよう設定
    connect(barAction, SIGNAL(triggered(bool)),
            this, SLOT(showNodeSetWindow()));

    // 「ヘルプ」アクション
    // ヘルプアクションの作成
    helpAction = new QAction(tr("&ヘルプ"), this);
    helpAction->setIcon(QIcon(":/images/question.png"));
    connect(helpAction, SIGNAL(triggered(bool)),
            this, SLOT(showHelp()));
}

void MainWindow::createMenus()
{
    // メニューの作成
    dispMenu = menuBar()->addMenu(tr("&表示"));
    dispMenu->addAction(visualAction);
    dispMenu->addAction(toolBarAction);
    dispMenu->addAction(colorAction);
    dispMenu->addAction(detailAction);
    dispMenu->addAction(rowDataAction);

    // キャプチャメニュー
    pcapMenu = menuBar()->addMenu(tr("&キャプチャ"));
    pcapMenu->addAction(deviceAction);
    pcapMenu->addAction(filterAction);
    pcapMenu->addAction(startAction);
    pcapMenu->addAction(stopAction);
    pcapMenu->addAction(toTopAction);
    pcapMenu->addAction(toBottomAction);
    pcapMenu->addAction(autoScrollAction);
    pcapMenu->addAction(searchAction);

    // 分析メニュー
    analyzeMenu = menuBar()->addMenu(tr("&分析"));
    analyzeMenu->addAction(circleAction);
    analyzeMenu->addAction(seriesAction);
    analyzeMenu->addAction(barAction);
    analyzeMenu->addAction(logAction);

    // ヘルプメニュー
    helpMenu = menuBar()->addMenu(tr("&ヘルプ"));
    helpMenu->addAction(helpAction);
}

void MainWindow::createToolBar()
{
    // ツールバーの作成
    mainToolBar = addToolBar(tr("&ツールバー"));
    mainToolBar->addAction(startAction);    // 開始
    mainToolBar->addAction(stopAction);     // 停止
    mainToolBar->addAction(toTopAction);    // スクロールツートップ
    mainToolBar->addAction(toBottomAction); // スクロールツーボトム
    mainToolBar->addAction(autoScrollAction);   // 自動スクロール
    mainToolBar->addAction(searchAction);   // 検索
    mainToolBar->addAction(circleAction);   // プロトコル統計
    mainToolBar->addAction(seriesAction);   // トラフィック量統計
    mainToolBar->addAction(barAction);      // ノード統計
    mainToolBar->addAction(logAction);      // ログ
    mainToolBar->addAction(helpAction);     // ヘルプ
}

void MainWindow::setupModel()
{
    model = new QStandardItemModel(0,7);    // モデルの作成
    // モデルのヘッダ作成
    model->setHeaderData(0, Qt::Horizontal, tr("番号"));
    model->setHeaderData(1, Qt::Horizontal, tr("取得時刻"));
    model->setHeaderData(2, Qt::Horizontal, tr("送信元IPアドレス"));
    model->setHeaderData(3, Qt::Horizontal, tr("宛先IPアドレス"));
    model->setHeaderData(4, Qt::Horizontal, tr("パケット長"));
    model->setHeaderData(5, Qt::Horizontal, tr("プロトコル"));
    model->setHeaderData(6, Qt::Horizontal, tr("備考"));
}

void MainWindow::setupView()
{
    table = new QTableView; // ビューの作成
    detailText = new QTextEdit();  // パケット詳細ラベルの作成
    rowDataText = new QTextEdit();  // 生パケットラベルの作成
    detailText->setReadOnly(true);   // パケット詳細を読み取りのみにする
    rowDataText->setReadOnly(true);   // 生パケットを読み取りのみにする

    search = new SearchDialog;  // 検索ウィンドウの作成
    LWindow = new LogWindow;    // ログウィンドウの作成

    if (dbconnection)   // データベースに接続できる場合
        LWindow->setDBaccess(DB);   // データベースアクセス設定

    // パケット詳細のドックウィジット（レイアウト：ボトム）生成
    detailWidget = new QDockWidget(tr("パケット詳細"));
    detailWidget->setWidget(detailText);
    detailWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, detailWidget);
    connect(detailWidget, SIGNAL(visibilityChanged(bool)),
            detailAction, SLOT(setChecked(bool)));

    // 生パケットデータのドックウィジット（レイアウト：ボトム）生成
    rowDataWidget = new QDockWidget(tr("生パケットデータ"));
    rowDataWidget->setWidget(rowDataText);
    rowDataWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, rowDataWidget);
    connect(rowDataWidget, SIGNAL(visibilityChanged(bool)),
            rowDataAction, SLOT(setChecked(bool)));

    table->setModel(model); // ビューへのモデルの指定
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 表の編集不可
    // セルクリック時に行を選択するように設定
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // パケット一覧のスクロールバーは常にON
    table->setAutoScroll(true);

    // アイテム選択モデルは，テーブルの選択が変更されたときや，自分で選択するときに使用する
    selectionModel = new QItemSelectionModel(model);    // アイテム選択モデルの設定
    table->setSelectionModel(selectionModel);

    // アイテム選択時のシグナルとスロットを接続
    connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(packetSelected(QModelIndex)));

    // 検索に関するシグナルとスロットを接続
    connect(search, SIGNAL(searchButtonClicked()),
            this, SLOT(doSearch()));
    connect(search, SIGNAL(previousButtonClicked()),
            this, SLOT(scrollToPrevious()));
    connect(search, SIGNAL(nextButtonClicked()),
            this, SLOT(scrollToNext()));
    connect(search, SIGNAL(hided()),
            this, SLOT(finishSearch()));

    table->setColumnWidth(0, 60); // 番号の幅を調節
    table->setColumnWidth(1, 60); // 取得時間の幅を調節
    table->setColumnWidth(2, 125); // 送信元IPアドレスの幅を調節
    table->setColumnWidth(3, 125); // 宛先IPアドレスの幅を調節
    table->setColumnWidth(4, 90);   // パケット長の幅を調節
    table->setColumnWidth(5, 90); // プロトコルの幅を調節

    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->hide();

    setCentralWidget(table);    // セントラルウィジット設定
}

void MainWindow::setPacketData(QStringList &data)
{
    if (data.count()) { // データが存在する場合
        int row = maxrow++; // 挿入する行
        packetList.insert(row, data); // 新しくデータを追加
        model->insertRows(row, 1, QModelIndex());   // 表の末尾に1行追加
        model->setData(model->index(row, 0, QModelIndex()),
                       data[0]);    // パケットID
        if (data.count() == 14 && colorable)
            model->setData(model->index(row, 0, QModelIndex()),
                           QColor(data[13]), Qt::BackgroundColorRole);  // 色
        for (int j = 1; j < 7; j++) {
            // 取得時間,送信元,宛先,パケット長,プロトコル,備考,13番目に色
            model->setData(model->index(row, j, QModelIndex()),
                           data[j + 1]);  // データの追加
            if (data.count() == 14 && colorable)
                model->setData(model->index(row, j, QModelIndex()),
                               QColor(data[13]), Qt::BackgroundColorRole);  // 色
        }
        if (autoscrollable) // 自動スクロールオンのとき
            scrollToBottom();   // ボトムへスクロール
    }
}

void MainWindow::dispDetail(int row)
{
    if (row == -1)  // 検索結果のデータを表示する場合
        row = searchResult[nowRowIndex];    // 選択中の行
    if (row >= packetList.count())  // 行が存在しない場合
        return;

    QString qsdata;
    qsdata.clear(); // 文字列初期化
    qsdata += tr("パケットID [") + packetList[row][0] + tr("]\n");
    for (int i = 8; i < 12; i++)    // 各層の詳細を取得
        qsdata += packetList[row][i];
    detailText->setText(qsdata); // パケット詳細を表示
    qsdata.clear(); // 文字列初期化
    qsdata += tr("パケットID [") + packetList[row][0] + tr("]\n");
    qsdata += packetList[row][12];  // 生データ
    rowDataText->setText(qsdata);  // 生データを表示
}

void MainWindow::dispResultRows()
{
    // すべての検索結果に色付けをする
    foreach (const int &row, searchResult) {
        for (int j = 0; j < 7; j++)
            model->setData(model->index(row, j, QModelIndex()),
                           QColor(Qt::gray), Qt::BackgroundColorRole);
    }
}

void MainWindow::sortNodesList()    // 挿入ソートを行う
{
    QList<QStringList> result;
    for (int i = 0; i < nodesList.count(); i++) {   // すべてのノードの組み
        int j = 0;
        int size = result.count();
        while (j < size) {
            if (nodesList[i][2].toDouble() > result[j][2].toDouble())
                break;
            j++;
        }
        result.insert(j , nodesList[i]); // 降順で適切な位置に挿入
    }
    nodesList = result;  // 結果を代入
}

void MainWindow::showWarning(QString &text)
{
    QMessageBox warning;    // 警告メッセージ
    warning.setIcon(QMessageBox::Warning);  // アイコン表示
    warning.setText(text);  // 表示メッセージ設定
    warning.setStandardButtons(QMessageBox::Ok);
    warning.setDefaultButton(QMessageBox::Ok);
    warning.exec(); // 警告メッセージを表示
}

bool MainWindow::showLogSaveWarning()
{
    // 警告メッセージ
    QMessageBox warning;
    warning.setText(tr("パケットキャプチャ統計ログを保存していません"));
    warning.setInformativeText(tr("ログを保存しますか？"));
    warning.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                               QMessageBox::Cancel);    // ボタン表示
    warning.setIcon(QMessageBox::Question);  // アイコン表示
    warning.setDefaultButton(QMessageBox::Save);

    // 警告メッセージ表示
    int msg = warning.exec();
    switch (msg) {
        case QMessageBox::Save:
            saveLog();  // ログを保存する
            if (!saved) // 保存されていない場合
                return false;   // キャンセル
            break;
        case QMessageBox::Discard:
            // 保存せずに終了
            break;
         case QMessageBox::Cancel:
            // キャンセル
            return false; break;
        default:
            return false; break;
    }
    return true;
}

void MainWindow::saveLog()
{
    // ログ保存ダイアログ表示
    LogSaveDialog *LSDialog = new LogSaveDialog;
    if (LSDialog->exec() == QDialog::Accepted) {
        QString name = LSDialog->logName();
        if (!DB->insertLogData(name)) {  // ログデータ保存
            QString msg("ログデータの保存に失敗しました");
            showWarning(msg);
            DB->closeConnection();
            return;
        }
        if (!DB->insertNodeSet(nodesList)) { // ノードの組み保存
            QString msg("ノードの組み統計データの保存に失敗しました");
            showWarning(msg);
            DB->closeConnection();
            return;
        }
        if (!DB->insertProtocol(protocolList)) { // プロトコル保存
            QString msg("プロトコル統計データの保存に失敗しました");
            showWarning(msg);
            DB->closeConnection();
            return;
        }
        if (!DB->insertTraffic(trafficMap)) {    // トラフィック量保存
            QString msg("トラフィック量統計データの保存に失敗しました");
            showWarning(msg);
            DB->closeConnection();
            return;
        }
        saved = true;   // 保存完了!
        if (saved)
            std::cout<<"保存を完了しました"<<std::endl;
    }
}
