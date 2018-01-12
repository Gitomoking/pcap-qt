#ifndef VISUALIZATIONWINDOW_H
#define VISUALIZATIONWINDOW_H

#include "vnode.h"
#include "vpacket.h"
#include "vinterface.h"
#include <QGraphicsItem>
#include <QMap>
#include <QWidget>
#include <QSpinBox>
#include <QTextEdit>
#include <QPushButton>

class QGraphicsView;
class QGraphicsScene;
class QGraphicseEllipseItem;
class QGraphicsProxyWidget;
class QLabel;

class VisualizationWindow : public QWidget
{
    Q_OBJECT

public:
    VisualizationWindow(QWidget *parent=0);
    void addData(QStringList &data);  /* データの追加 */
    void clearWindow(); /* ウィンドウおよびデータの初期化 */

protected:
    void closeEvent();

private:
    void createWidgets();
    void createProxyWidgets();
    void createLayout();
    void createView();
    bool checkPointF(QPointF &checkedP);
    VNode *createNode(QString &nodeName);
    VInterface *createInterface(QString &sName, QString &dName,
                                QPointF &sNodeP, QPointF &dNodeP);
    VPacket *createPacket(QStringList &data,
                          VInterface *interface);

    bool start;
    QGraphicsView *view;    /* ビュー */
    QGraphicsScene *scene;  /* シーン */
    QGraphicsRectItem *viewItem;    /* ビューポート */
    QLabel *leftNodeCountLabel;     /* ノード数ラベル */
    QLabel *rightNodeCountLabel;    /* ノード数の数値ラベル */
    QLabel *maxNodeCountLabel;      /* 最大ノード数ラベル */
    QSpinBox *nodeCountSpinBox;     /* 最大ノード数スピンボックス */
    QPushButton *showNameButton;    /* IPアドレス表示／非表示ボタン */
    QPushButton *showInterfaceButton;   /* インタフェース表示／非表示ボタン */
    QPushButton *showPacketButton;  /* パケット表示／非表示ボタン */
    QPushButton *deleteButton;      /* 削除ボタン */
    QPushButton *settingsButton;    /* 設定ボタン */
    QTextEdit *itemDetail;          /* アイテム詳細テキスト */

    QMap<QString, QGraphicsProxyWidget*> proxyForName;

    QMap<QString, int> nodeNames;   /* ノード名 */
    QList<QPointF> nodePoint;   /* ノードの中心座標 */
    QList<VNode*> nodeList;     /* ノード */
    QMap<QStringList, VInterface*> interfaceMap;    /* インタフェース */

    int nodeWidth;      /* ノード（円）の幅 */
    int nodeHeight;     /* ノード（円）の高さ */
    int minXDistance;   /* ノードのx座標間の最小距離 */
    int minYDistance;   /* ノードのy座標間の最小距離 */
    int maxnode;
};

#endif // VISUALIZATIONWINDOW_H
