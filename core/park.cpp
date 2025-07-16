#include "park.h"
#include <QPainter>

Park::Park(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent):
    MapItem{id, points},
    QGraphicsObject{parent}
{
    if(d_points.empty())
    {
        qDebug() << "No points available to draw the park.";
    }
    else
    {

        for(const QPointF& p: d_points)
        {
            d_polygon << p;
        }
    }
}

QRectF Park::boundingRect() const
{
    return d_polygon.boundingRect();
}

QPainterPath Park::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Park::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto originalBrush  = painter->brush();
    auto originalPen    = painter->pen();

    QBrush brush{QColor(205, 235, 176)};
    QPen pen{QColor(172, 225, 120), 1}; // Noir, épaisseur 1 pixel

    painter->setPen(pen);
    painter->setBrush(brush);

    painter->drawPolygon(d_polygon);

    painter->setBrush(originalBrush);
    painter->setPen(originalPen);
}
