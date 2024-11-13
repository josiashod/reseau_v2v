#include "water.h"

Water::Water(long long id): MapItem{id}
{}

void Water::draw(QGraphicsItemGroup* group) const
{
    if(d_points.empty())
    {
        qDebug() << "No points available to draw the park.";
        return;
    }

    QPolygonF polygon;

    for(const QPointF& p: d_points)
    {
        polygon << p;
    }

    if (!polygon.isClosed())
    {
        polygon << polygon.first();
    }

//    QPainterPath p;
//    p.addPolygon(polygon);

    auto water = new QGraphicsPolygonItem(polygon);
    QBrush brush{QColor(0, 191, 255)};
    water->setBrush(brush);
    QPen pen{QColor(0, 191, 255), 1}; // Noir, épaisseur 1 pixel
    water->setPen(pen);

    group->addToGroup(water);
}
