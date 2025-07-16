#include "building.h"
#include <QPainter>

Building::Building(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent):
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

QRectF Building::boundingRect() const
{
    return d_polygon.boundingRect();
}

QPainterPath Building::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Building::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto originalBrush  = painter->brush();
    auto originalPen    = painter->pen();

    QBrush brush{QColor(216, 208, 201)};
    QPen pen{QColor(190, 190, 189), 1};

    painter->setPen(pen);
    painter->setBrush(brush);

    painter->drawPolygon(d_polygon);

    painter->setBrush(originalBrush);
    painter->setPen(originalPen);
}
