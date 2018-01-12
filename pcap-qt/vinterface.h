#ifndef VINTERFACE_H
#define VINTERFACE_H

#include <QGraphicsItem>

class QPainter;

class VInterface : public QGraphicsItem
{
public:
    enum {Type = UserType + 2};

    VInterface(QGraphicsItem *parent);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const { return Type; }

    QPointF sourceNodePoint() const { return sPoint; }
    QPointF destiNodePoint() const { return dPoint; }
    QString sourceNodeName() const { return sName; }
    QString destiNodeName() const { return dName; }
    ulong nowPackets() const { return packets; }
    ulong nowBytes() const { return bytes; }
    QString detail();
    void addPacket(QStringList &data);
    void setNodes(QString &sNode, QString &dNode,
                  QPointF &sNodeP, QPointF &dNodeP);
private:
    QString sName;  /* 送信元ノード名 */
    QString dName;  /* 宛先ノード名 */
    QPointF sPoint; /* 送信元ノード中心点 */
    QPointF dPoint; /* 宛先ノード中心点 */
    ulong packets;    /* パケット数 */
    ulong bytes;      /* バイト数 */
};

#endif // VINTERFACE_H
