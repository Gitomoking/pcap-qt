#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "visualizationwindow.h"
#include "devicewindow.h"
#include "searchdialog.h"
#include "logwindow.h"
#include <QMainWindow>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QTableView>
#include <QMenu>
#include <QMenuBar>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QProcess>
#include <QToolBar>
#include <QIcon>
#include <QApplication>
#include <QPixmap>

#define READ_MAINPACKET    1
#define READ_PROTOCOL      2
#define READ_TRAFFIC       3
#define READ_NODES         4
#define READ_END           5
#define READ_DEVICE        6

#define MAX_PROTOCOL 13

class QAction;
class QTextEdit;
class QLabel;

const QString errorColor("#ff0000");
const QString arpColor("#ff7f7f");
const QString tcpColor("#00ff7f");
const QString udpColor("#00bfff");
const QString icmpColor("#b114ff");
const QString dnsColor("#adff2f");
const QString httpColor("#ffff00");
const QString httpsColor("#ffff00");
const QString ftpColor("#006400");
const QString ntpColor("#0000ff");
const QString dhcpColor("#ffc0cb");
const QString telnetColor("#ffffe0");
const QString sshColor("#ff1493");
const QString otherColor("#fafafa");

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    void showVWindow(); // 可視化ウィンドウの表示
    void start();   // パケットキャプチャの開始
protected:
    void closeEvent(QCloseEvent *event);    // ウィンドウが閉じるときに実行される
private slots:
    void setCaptureDevice(int index); // キャプチャデバイスを設定
    void switchVWindow();   // 可視化ウィンドウの表示切り替え
    void setToolBarVisible();   // ツールバーの表示／非表示
    void setPacketColorVisible();   // パケットの色分け
    void setDetailVisible();    // パケット詳細の表示／非表示
    void setRowDataVisible();   // 生パケットデータの表示／非表示
    void scrollToTop(); // 先頭パケットへスクロール
    void scrollToBottom();  // 末尾パケットへスクロール
    void scrollToPrevious();    // 検索の前パケットへスクロール
    void scrollToNext();    // 検索の次パケットへスクロール
    void setAutoScroll();   // 自動スクロール
    void readData();    // 子プロセスの標準出力からデータを読み出す
    void packetSelected(QModelIndex current);   // パケット選択時の処理
    void startPacketCapture();  // パケットキャプチャを実行
    void stopPacketCapture();   // パケットキャプチャを停止
    void doSearch();    // 検索を実行
    void finishSearch();    // 検索終了
    void showDeviceWindow();    // デバイス選択ウィンドウの表示
    void showFilterWindow();    // フィルタ式確認ウィンドウの表示
    void showSearchDialog();    // 検索ウィンドウの表示
    void showProtocolWindow();  // プロトコル統計ウィンドウの表示
    void showTrafficWindow();   // トラフィック量統計ウィンドウの表示
    void showNodeSetWindow();  // ノードの組み統計ウィンドウの表示
    void showLogWindow();   // ログ一覧ウィンドウの表示
    void showHelp();        // ヘルプの表示
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
private:
    void createActions();   // アクションの作成
    void createMenus(); // メニューの作成
    void createToolBar();  // ツールバーの作成
    void setupModel();  // モデルの設定
    void setupView();   // テーブルビューの設定
    void setPacketData(QStringList &data);    // テーブルにデータを挿入
    void dispDetail(int row);   // パケット詳細を表示
    void dispResultRows();    // 検索結果の行を選択状態にする
    void sortNodesList();   // ノードリストのソート
    void showWarning(QString &text);    // 警告メッセージを表示
    bool showLogSaveWarning();  // ログ保存の警告メッセージを表示
    void saveLog();     // ログを保存

    int captureCount;   // キャプチャを実行した回数
    int maxrow; // テーブルに挿入されているデータ数
    bool colorable; // 色分けをするかどうか
    bool autoscrollable;    // 自動スクロールをするかどうか
    bool visualable;    // 可視化ウィンドウを表示するかどうか
    bool running;   // キャプチャ中かどうか
    bool saved;     // ログが保存済みかどうか
    bool dbconnection;  // データベース接続があるかどうか
    bool searching; // 検索中かどうか
    bool deleteFlag;    // 行削除フラグ
    QString filExp; // フィルタ式
    QString device; // 現在のデバイス
    QList<int> searchResult;    // 検索結果
    int nowRowIndex;    // 検索結果で表示している行（リストの何番目か）
    QList<QStringList> deviceList;      // デバイス一覧
    QList<QStringList> packetList;      // パケットデータ
    QList<QStringList> protocolList;    // プロトコル統計データ
    QList<QStringList> nodesList;       // ノードの組み統計データ
    QMap<QStringList, QList<double_t>> trafficTemp; // トラフィック量
    QMap<int, QList<double_t> > trafficMap;  // トラフィック量統計データ

    QAbstractItemModel *model;  // テーブル用データモデル
    QStandardItemModel *devModel;   // デバイス一覧用データモデル
    QItemSelectionModel *selectionModel;    // アイテム選択モデル
    QTableView *table;          // テーブルビュー
    QTextEdit *detailText;           // パケット詳細表示用
    QTextEdit *rowDataText;           // 生パケット表示用
    QDockWidget *detailWidget;  // パケット詳細ウィジット
    QDockWidget *rowDataWidget; // 生パケットデータウィジット

    DBAccess *DB;               // データベース接続
    QProcess *process;          // 子プロセスとのインタフェース

    SearchDialog *search;   // 検索ダイアログ
    LogWindow *LWindow;     // ログウィンドウ
    DeviceWindow *DWindow;  // デバイス選択ウィンドウ
    VisualizationWindow *VWindow;   // 可視化ウィンドウ

    // メニューバー
    QMenu *dispMenu;    // 表示メニュー
    QMenu *pcapMenu;    // キャプチャメニュー
    QMenu *analyzeMenu; // 分析メニュー
    QMenu *helpMenu;   // ヘルプメニュー

    // ツールバー
    QToolBar *mainToolBar;

    // アクション
    QAction *visualAction;   // 可視化ウィンドウ表示アクション
    QAction *toolBarAction; // ツールバー表示／非表示アクション
    QAction *colorAction;   // パケットの色分け設定アクション
    QAction *detailAction;  // パケット詳細の表示アクション
    QAction *rowDataAction; // 生パケットデータの表示アクション

    QAction *stopAction;    // 停止アクション
    QAction *startAction;   // 開始アクション
    QAction *deviceAction;  // デバイス一覧アクション
    QAction *filterAction;  // フィルタ式確認アクション
    QAction *toTopAction;   // 最上パケットまでスクロールするアクション
    QAction *toBottomAction;    // 最下パケットまでスクロールするアクション
    QAction *autoScrollAction;  // 自動スクロールON/OFFアクション
    QAction *searchAction;  // 検索アクション

    QAction *circleAction;  // 円グラフアクション
    QAction *seriesAction;  // 折れ線グラフアクション
    QAction *barAction;     // 棒グラフアクション
    QAction *logAction;     // ログアクション

    QAction *helpAction;    // ヘルプアクション
};

#endif // MAINWINDOW_H
