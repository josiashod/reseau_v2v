#include "water.h"
#include <QPainter>

Water::Water(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent):
    MapItem{id, points},
    QGraphicsObject{parent}
{
    if(d_points.empty())
    {
        qDebug() << "No points available to draw the water.";
    }
    else
    {
        for(const QPointF& p: d_points)
        {
            d_polygon << p;
        }

        // // Ensure the polygon is closed
        // if (d_polygon.first() != d_polygon.last()) {
        //     d_polygon << d_polygon.first();
        // }
    }
}

QRectF Water::boundingRect() const
{
    return d_polygon.boundingRect();
}

QPainterPath Water::shape() const
{
    QPainterPath path;
    path.addPolygon(d_polygon);
    return path;
}

void Water::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto originalBrush  = painter->brush();
    auto originalPen    = painter->pen();

    QBrush brush{QColor(0, 191, 255)};
    QPen pen{QColor(0, 191, 255), 8};

    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    painter->setPen(pen);
    painter->setBrush(brush);

    painter->drawPolyline(d_polygon);

    painter->setBrush(originalBrush);
    painter->setPen(originalPen);
}
