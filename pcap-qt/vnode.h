#ifndef VNODE_H
#define VNODE_H

#include <QGraphicsItem>

class QPainter;

class VNode : public QGraphicsItem
{
public:
    enum {Type = UserType + 1};

    VNode(QGraphicsItem *parent);

    QRectF boundingRect() const { return nodeRect; }
    QPainterPath shape() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const { return Type; }

    QPointF midPoint();
    QString name() const { return nodeName; }
    void setNodeData(QString &nname, QRectF &rect);
private:
    QString nodeName;
    QRectF nodeRect;
};

#endif // VNODE_H
