#ifndef VPACKET_H
#define VPACKET_H

#include <QGraphicsObject>

class QPainter;

class VPacket : public QGraphicsObject
{

public:
    enum { Type = UserType + 3 };

    VPacket(QGraphicsItem *parent=0);

    QRectF boundingRect() const { return packetPath.boundingRect(); }
    QPainterPath shape() const { return packetPath; }
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const { return Type; }

    QString detail();
    void setPacketData(QStringList &data,
                       QPointF &sNodeP, QPointF &dNodeP);

protected:
    void advance(int step) override;

private:
    int pid;  /* パケットID */
    bool notError;  /* エラー */
    QString sName;  /* 送信元IPアドレス */
    QString dName;  /* 宛先IPアドレス */
    int bytes;      /* パケット長 */
    QString proto;  /* プロトコル */

    QPointF velocity;   /* 速度 */
    QPointF nowPoint;   /* 現在の座標 */
    QPointF sPoint; /* 送信元座標(左上) */
    QPointF dPoint; /* 宛先座標(左上) */
    QPainterPath packetPath;
};


#endif // VPACKET_H
