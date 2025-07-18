#include "water.h"
#include <QPainter>

Water::Water(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent):
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

        if (!d_polygon.isClosed())
        {
            d_polygon << d_polygon.first();
        }
    }
}

QRectF Water::boundingRect() const
{
    return d_polygon.boundingRect();
}

QPainterPath Water::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Water::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto originalBrush  = painter->brush();
    auto originalPen    = painter->pen();

    QBrush brush{QColor(0, 191, 255)};
    QPen pen{QColor(0, 191, 255), 1}; // Noir, épaisseur 1 pixel

    painter->setPen(pen);
    painter->setBrush(brush);

    painter->drawPolygon(d_polygon);

    painter->setBrush(originalBrush);
    painter->setPen(originalPen);
}
