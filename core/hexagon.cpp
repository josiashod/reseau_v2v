#include "hexagon.h"
#include <QPainter>
#include <QTimer>

Hexagon::Hexagon(QPolygonF hexagon, QGraphicsItem *parent):
    QGraphicsObject{parent}, d_polygon{hexagon}
{}

QRectF Hexagon::boundingRect() const
{
    return d_polygon.boundingRect();
}

QPainterPath Hexagon::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Hexagon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto originalPen    = painter->pen();
    QPen pen{QColor(0, 0, 0), 1};

    painter->setPen(pen);
    painter->drawPolygon(d_polygon);

    painter->setPen(originalPen);
}
