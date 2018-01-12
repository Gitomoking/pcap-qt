#include "visualizationwindow.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QLabel>
#include <QTimer>
#include <iostream>
const int viewRectWidth = 600;
const int viewRectHeight = 600;

VisualizationWindow::VisualizationWindow(QWidget *parent)
{
    scene = new QGraphicsScene(this);   // シーンの作成
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    nodeWidth = 20;     /* 円の直径 */
    nodeHeight = 20;    /* 円の直径 */
    minXDistance = nodeWidth * 2;  /* x最小距離 */
    minYDistance = nodeHeight * 2; /* y最小距離 */
    maxnode = 30;   /* 最大ノード数 */
    start = false;

    createWidgets();    // ウィンドウのウィジット生成
    createProxyWidgets();   // シーンへのウィジットの追加
    createLayout(); // レイアウトの設定
    createView();   // ビューの生成

    resize(822, 622);   // ウィンドウサイズ
    setWindowTitle(tr("可視化ウィンドウ")); // ウィンドウタイトル
}

void VisualizationWindow::closeEvent()
{
    // ウィンドウが閉じられたとき，生成したリストを削除する
    start = false;
    qDeleteAll(nodeList);
    qDeleteAll(interfaceMap);
}

void VisualizationWindow::createWidgets()
{
    // ウィジットの生成
    QString styleSheet("background-color: gray;");
    leftNodeCountLabel = new QLabel(tr("ノード数 : ")); // ノード数ラベル
    leftNodeCountLabel->setStyleSheet(styleSheet);  // 背景色設定
    leftNodeCountLabel->setMaximumWidth(80);    // 最大幅設定
    rightNodeCountLabel = new QLabel;   // ノード数の数値ラベル
    rightNodeCountLabel->setStyleSheet(styleSheet);
    rightNodeCountLabel->setMaximumWidth(50);
    maxNodeCountLabel = new QLabel(tr("最大ノード数"));   // 最大ノード数ラベル
    maxNodeCountLabel->setStyleSheet(styleSheet);
    maxNodeCountLabel->setMaximumWidth(80);
    nodeCountSpinBox = new QSpinBox;    // 最大ノード数スピンボックス
    nodeCountSpinBox->setRange(1,100);  // 表示範囲設定
    nodeCountSpinBox->setValue(60); // 初期値設定
    nodeCountSpinBox->setMaximumWidth(50);
    nodeCountSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    nodeCountSpinBox->setDisabled(true);

    // ボタン生成
    showNameButton = new QPushButton(tr("ノード名非表示"));
    showNameButton->setMaximumWidth(150);
    showNameButton->setDisabled(true);
    showInterfaceButton = new QPushButton(tr("インタフェース非表示"));
    showInterfaceButton->setMaximumWidth(150);
    showInterfaceButton->setDisabled(true);
    showPacketButton = new QPushButton(tr("パケット非表示"));
    showPacketButton->setMaximumWidth(150);
    showPacketButton->setDisabled(true);
    deleteButton = new QPushButton(tr("削除"));
    deleteButton->setMaximumWidth(150);
    deleteButton->setDisabled(true);
    settingsButton = new QPushButton(tr("設定"));
    settingsButton->setMaximumWidth(150);
    settingsButton->setDisabled(true);
    itemDetail = new QTextEdit(tr("情報（未実装）"));
    itemDetail->setMaximumWidth(180);
}

void VisualizationWindow::createProxyWidgets()
{
    // シーンにウィジットを追加
    proxyForName["leftNodeCountLabel"] = scene->addWidget(
                leftNodeCountLabel);
    proxyForName["rightNodeCountLabel"] = scene->addWidget(
                rightNodeCountLabel);
    proxyForName["maxNodeCountLabel"] = scene->addWidget(
                maxNodeCountLabel);
    proxyForName["nodeCountSpinBox"] = scene->addWidget(
                nodeCountSpinBox);
    proxyForName["showNameButton"] = scene->addWidget(
                showNameButton);
    proxyForName["showInterfaceButton"] = scene->addWidget(
                showInterfaceButton);
    proxyForName["showPacketButton"] = scene->addWidget(
                showPacketButton);
    proxyForName["deleteButton"] = scene->addWidget(deleteButton);
    proxyForName["settingsButton"] = scene->addWidget(settingsButton);
    proxyForName["itemDetail"] = scene->addWidget(itemDetail);
}

void VisualizationWindow::createLayout()
{
    // レイアウトの設定
    QGraphicsLinearLayout *leftLayout = new QGraphicsLinearLayout(
                Qt::Vertical);
    foreach (const QString &name, QStringList()
             << "leftNodeCountLabel" << "rightNodeCountLabel"
             << "maxNodeCountLabel" << "nodeCountSpinBox"
             << "showNameButton" << "showInterfaceButton"
             << "showPacketButton" << "deleteButton"
             << "settingsButton" << "itemDetail")
        leftLayout->addItem(proxyForName[name]);

    // メインのレイアウト
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(
                Qt::Horizontal);
    layout->addItem(leftLayout);    /* 情報表示用 */
    layout->setItemSpacing(0, viewRectWidth);
    layout->addStretch(false);

    // QGraphicsWidgetの生成とシーンへの追加
    QGraphicsWidget *widget = new QGraphicsWidget;
    widget->setLayout(layout);
    scene->addItem(widget);

    int width = qRound(layout->preferredWidth());
    int height = viewRectHeight;
    setMinimumSize(width, height);
    scene->setSceneRect(0, 0, width, height);   // シーンのサイズ設定
}

void VisualizationWindow::createView()
{
    // 可視化アイテムを表示するビューポートの作成
    viewItem = new QGraphicsRectItem;
    viewItem->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    viewItem->setPen(QPen(QColor("green"), 1.0));
    viewItem->setBrush(Qt::black);
    /* (x,y)=(200, 0)に，viewRectWidth*viewRectHeightの矩形を設定 */
    viewItem->setRect(201, 1, viewRectWidth, viewRectHeight);

    scene->addItem(viewItem);

    // ビューの作成
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QColor("gray"));
    view->resize(822,622);
    view->setWindowTitle(tr("可視化ウィンドウ"));
}

void VisualizationWindow::addData(QStringList &data)
{  
    /* dataの形式（MainWindowから受け取る）
     * パケットID,エラー情報,送信元,宛先,パケット長,プロトコル */
    if (!nodeNames.count() || !nodeNames.contains(data[2])) {    /* 送信元ノードが存在しない場合 */
        VNode *node = createNode(data[2]);  /* 新規ノード作成 */
        nodeList.append(node);
    }
    if (!nodeNames.contains(data[3])) {    /* 宛先ノードが存在しない場合 */
        VNode *node = createNode(data[3]);  /* 新規ノード作成 */
        nodeList.append(node);
    }
    QPointF sPoint = nodePoint.at(nodeNames[data[2]]);
    QPointF dPoint = nodePoint.at(nodeNames[data[3]]);
    QStringList ifKey = QStringList() << data[2] << data[3];
    if (!interfaceMap.count() || !interfaceMap.contains(ifKey)) {
        /* ノード間インタフェースが存在しない場合 */
        VInterface *interface = createInterface(data[2], data[3],
                sPoint, dPoint);
        interfaceMap.insert(ifKey, interface);
    }
    VInterface *packetif = interfaceMap[ifKey];
    VPacket *packet = createPacket(data, packetif);
    scene->invalidate();
    view->update();

    if (!start) {
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), scene, SLOT(advance()));
        connect(timer, SIGNAL(timeout()), scene, SLOT(update()));
        timer->start(30);
        start = true;
    }
}

VNode *VisualizationWindow::createNode(QString &nodeName)
{
    VNode *node = new VNode(viewItem);  /* 新規ノード */
    QPointF nodeP;  /* ノードの左上座標 */
    do {
        // (x, y)を乱数で決定
        qreal x = 200 + nodeWidth * 2 + qrand() % (viewRectWidth - nodeWidth * 4);
        qreal y = qrand() % (viewRectHeight - nodeHeight * 2);
        nodeP.setX(x);  /* 左上x */
        nodeP.setY(y);  /* 左上y */
    } while(!checkPointF(nodeP) && (nodePoint.count() < maxnode));    /* 不適当な座標なら繰り返し */
    QRectF nodeRect(nodeP, QSizeF(nodeWidth, nodeHeight));  /* ノードの境界矩形 */
    node->setNodeData(nodeName, nodeRect);   /* ノードの情報を設定 */
    nodePoint.append(node->midPoint()); /* ノードの中心点 */
    nodeNames.insert(nodeName, nodePoint.indexOf(node->midPoint())); /* ノード座標情報を追加 */
    return node;
}

VInterface *VisualizationWindow::createInterface(QString &sName, QString &dName,
                                          QPointF &sNodeP, QPointF &dNodeP)
{
    VInterface *interface = new VInterface(viewItem);   /* 新規インタフェース */
    interface->setNodes(sName, dName, sNodeP, dNodeP);  /* インタフェースの情報設定 */
    return interface;
}

VPacket *VisualizationWindow::createPacket(QStringList &data,
                                    VInterface *interface)
{
    VPacket *packet = new VPacket(viewItem);    /* 新規パケット */
    QPointF sNodeP = interface->sourceNodePoint();  /* 送信元座標 */
    QPointF dNodeP = interface->destiNodePoint();   /* 宛先座標 */
    packet->setPacketData(data, sNodeP, dNodeP);    /* パケットのデータを設定 */
    interface->addPacket(data); /* インタフェースのデータ */
    return packet;
}

bool VisualizationWindow::checkPointF(QPointF &checkedP)
{
    /* checkedP.x() - minXDistance より大きく
     * checkedP.x() + minXDistance より小さく
     * checkedP.y() - minYDistance より大きく
     * checkedP.y() + nodeHeight + minYDistance より小さい
     * このときに限り，falseが返し，そうでない場合はtrueを返す */
    QPointF centerP;    /* 中心点の座標 */
    centerP.setX(checkedP.x() + nodeWidth / 2);
    centerP.setY(checkedP.y() + nodeHeight / 2);
    foreach (const QPointF &usingP, nodePoint) {
        if (usingP.x() < (checkedP.x() - minXDistance))
            continue;
        else if (usingP.x() > (checkedP.x() + minXDistance))
            continue;
        else if (usingP.y() < (checkedP.y() - minYDistance))
            continue;
        else if (usingP.y() > (checkedP.y() + minYDistance))
            continue;
        else
            return false;  /* 不適当な場合はfalseを返す */
    }
    return true;
}

void VisualizationWindow::clearWindow()
{
    // データクリア
    nodeNames.clear();
    nodePoint.clear();
    qDeleteAll(nodeList);
    qDeleteAll(interfaceMap);
    nodeList.clear();
    interfaceMap.clear();
    scene->invalidate();
    view->update();
    start = false;
}
