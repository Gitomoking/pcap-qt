#include "vpacket.h"
#include "vinterface.h"
#include <QPainter>
#include <QTimer>
#include <QtCore/qmath.h>
#include <QPropertyAnimation>

const qreal packetRadius = 3.0; /* パケット図形の高さ / 2 の値 */
const qreal interval = 150;  /* 移動回数 */

VPacket::VPacket(QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    setZValue(1);   /* Z値の設定 ノードより後ろ，インタフェースより前に表示 */
}

void VPacket::paint(QPainter *painter,
                    const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPainterPath path;
    /* 端が丸まった矩形のパス */
    QPointF ltPoint = sPoint;   /* 矩形の左上座標 */
    QPointF rbPoint = sPoint;   /* 矩形の右上座標 */
    ltPoint.setX(ltPoint.x() - packetRadius);
    ltPoint.setY(ltPoint.y() - packetRadius);
    rbPoint.setX(rbPoint.x() + packetRadius * 3);   /* x長めに */
    rbPoint.setY(rbPoint.y() + packetRadius);
    QRectF rect(ltPoint, rbPoint);  /* 矩形の座標と大きさ */
    path.addRoundRect(rect, 1.0);   /* 端丸まった矩形 */
    /* 色の設定(プロトコルとエラーの有無により場合分け) */
    painter->setPen(QPen(Qt::NoPen)); /* 図形の枠線 */
    painter->setBrush(QBrush(Qt::blue));    /* 図形の色 */
    if (notError == false) {
        painter->setBrush(QBrush(QColor(255,0,0)));    /* 図形の色を赤色に指定 */
    } else if(proto == "TCP") {
        painter->setBrush(QBrush(QColor(0,255,127)));    /* 図形の色を明るい緑色に指定 */
    } else if(proto == "UDP") {
        painter->setBrush(QBrush(QColor(0,191,255)));    /* 図形の色を明るい水色に指定 */
    } else if(proto == "ICMP") {
        painter->setBrush(QBrush(QColor(177,20,255)));    /* 図形の色を明るい紫色に指定 */
    } else if(proto == "ARP") {
        painter->setBrush(QBrush(QColor(255,127,127)));    /* 図形の色を薄めの赤色に指定 */
    } else if(proto == "DNS") {
        painter->setBrush(QBrush(QColor(173,255,47)));    /* 図形の色を黄緑色に指定 */
    } else if(proto == "HTTP") {
        painter->setBrush(QBrush(QColor(255,255,0)));    /* 図形の色を黄色に指定 */
    } else if(proto == "FTP") {
        painter->setBrush(QBrush(QColor(0,100,0)));    /* 図形の色を濃い緑色に指定 */
    } else if(proto == "HTTPS") {
        painter->setBrush(QBrush(QColor(255,255,0)));    /* 図形の色を黄に指定 */
    } else if(proto == "DHCP") {
        painter->setBrush(QBrush(QColor(255,192,203)));    /* 図形の色をピンク色に指定 */
    } else if(proto == "SNMP") {
        painter->setBrush(QBrush(QColor(255,69,0)));    /* 図形の色をオレンジ色に指定 */
    } else if(proto == "NTP") {
        painter->setBrush(QBrush(QColor(0,0,255)));    /* 図形の色を青色に指定 */
    } else if(proto == "Telnet") {
        painter->setBrush(QBrush(QColor(255,255,224)));    /* 図形の色を明るい黄色に指定 */
    } else if(proto == "SSH") {
        painter->setBrush(QBrush(QColor(255,20,147)));    /* 図形の色をマゼンタ色に指定 */
    } else {
        painter->setBrush(QBrush(QColor(255,255,255)));    /* 図形の色を白色に指定 */
    }

    qreal rad = qAtan((sPoint.y() - dPoint.y())/((sPoint.x() - dPoint.x())));
    qreal degree = rad * (180/3.14); //ラジアンを角度に直す
    painter->translate(sPoint);
    painter->rotate(degree);    // パケット回転
    painter->translate(-sPoint);

    packetPath = path;
    painter->drawPath(packetPath);  /* パス描画 */
}

void VPacket::advance(int step)
{
    if (!step) return;

    nowPoint.setX(nowPoint.x() + velocity.x()); // 次x座標
    nowPoint.setY(nowPoint.y() + velocity.y()); // 次y座標

    // 宛先まで行ったら，削除
    if (sPoint.x() > dPoint.x() && nowPoint.x() <= dPoint.x()) {
        deleteLater();
        return;
    } else if (sPoint.x() < dPoint.x() && nowPoint.x() >= dPoint.x()) {
        deleteLater();
        return;
    }

    /* 次座標に移動 */
    setPos(mapToParent(velocity.x(), velocity.y()));

}

QString VPacket::detail()
{
    QString str;
    str.clear();
    str += "タイプ : パケット\n";
    str += "パケットID : ";
    str += pid;
    str += "\n";
    str += "送信元ノード\n";
    str += "  " + sName + "\n";
    str += "宛先ノード\n";
    str += "  " + dName + "\n";
    str += "パケット長 : ";
    str += bytes;
    str += "バイト\n";
    str += "プロトコル : " + proto + "\n";
    str += (notError) ? "エラーなし\n" : "エラーあり\n";
    return str;
}

void VPacket::setPacketData(QStringList &data, QPointF &sNodeP, QPointF &dNodeP)
{
    pid = data[0].toInt();  /* パケットID */
    notError = (data[1] == "1") ? true : false; /* エラーの有無 */
    sName = data[2];    /* 送信元ノード名 */
    dName = data[3];    /* 宛先ノード名 */
    bytes = data[4].toInt();    /* パケット長 */
    proto = data[5];    /* プロトコル名 */

    nowPoint = sNodeP;
    sPoint = sNodeP;    /* 送信元座標 */
    dPoint = dNodeP;    /* 宛先座標 */

    /* 速度の計算 */
    velocity.setX((dPoint.x() - sPoint.x()) / interval);
    velocity.setY((dPoint.y() - sPoint.y()) / interval);
}
