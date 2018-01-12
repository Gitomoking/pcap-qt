#include "vnode.h"
#include "vinterface.h"
#include <QPainter>

VInterface::VInterface(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    packets = 0;
    bytes = 0;
    setZValue(0);
}

QRectF VInterface::boundingRect() const
{
    qreal rleft, rtop, rwidth, rheight;
    if (sPoint.x() < dPoint.x()) {
        rleft = sPoint.x();
        rwidth = dPoint.x() - sPoint.x();
    } else {
        rleft = dPoint.x();
        rwidth = sPoint.x() - dPoint.x();
    }
    if (sPoint.y() < dPoint.y()) {
        rtop = sPoint.y();
        rheight = dPoint.y() - sPoint.y();
    } else {
        rtop = dPoint.y();
        rheight = sPoint.y() - dPoint.y();
    }

    QRectF rect(rleft, rtop, rwidth, rheight);
    return rect;
}

QPainterPath VInterface::shape() const
{
    QPainterPath path;
    path.moveTo(sPoint);
    path.lineTo(dPoint);
    path.closeSubpath();
    return path;
}

void VInterface::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(Qt::white, 1, Qt::DotLine, Qt::FlatCap));
    painter->drawLine(sPoint, dPoint);
}

QString VInterface::detail()
{
    QString str;
    str.clear();
    str += "タイプ : インタフェース\n";
    str += "送信元ノード\n";
    str += "  " + sName + "\n";
    str += "宛先ノード\n";
    str += "  " + dName + "\n";
    str += "パケット数 : ";
    str += packets;
    str += "\n";
    str += "バイト数 : ";
    str += bytes;
    str += "バイト\n";
    return str;
}

void VInterface::setNodes(QString &sNode, QString &dNode,
                          QPointF &sNodeP, QPointF &dNodeP)
{
    sName = sNode;
    sPoint = sNodeP;
    dName = dNode;
    dPoint = dNodeP;
}

void VInterface::addPacket(QStringList &data)
{
    ulong packet = data[4].toULong();   /* パケット長 */

    packets++;  /* パケット数 */
    bytes += packet;    /* バイト数 */
}
