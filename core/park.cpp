#include "park.h"

Park::Park(long long id): MapItem{id}
{}

void Park::draw(QGraphicsItemGroup* group) const
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

    auto park = new QGraphicsPolygonItem(polygon, group);
//    park->setPolygon(polygon);
    QBrush brush{QColor(205, 235, 176)};
    QPen pen{QColor(172, 225, 120), 1}; // Noir, épaisseur 1 pixel
    park->setPen(pen);
    park->setBrush(brush);

    group->addToGroup(park);
}
