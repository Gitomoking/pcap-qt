#include "vnode.h"
#include <QPainter>

const QBrush circleBrush = QBrush(Qt::magenta);

VNode::VNode(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setZValue(2);
}

QPainterPath VNode::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void VNode::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(circleBrush);
    painter->drawEllipse(nodeRect);
    if (nodeName.size()) {
        painter->setPen(QPen(Qt::red));
        qreal x = midPoint().x() - nodeRect.width() * 3;
        qreal y = midPoint().y() + nodeRect.height() * 1.5;
        QPointF tlpoint(x, y);
        painter->drawText(tlpoint, nodeName);
    }
}

QPointF VNode::midPoint()
{
    qreal x, y; /* 中点の座標 */
    x = nodeRect.x() + nodeRect.width() / 2;
    y = nodeRect.y() + nodeRect.height() / 2;
    return QPointF(x, y);
}

void VNode::setNodeData(QString &nname, QRectF &rect)
{
    nodeName = nname;
    nodeRect = rect;
}
