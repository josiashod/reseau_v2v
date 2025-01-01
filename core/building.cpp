#include "building.h"

Building::Building(long long id): MapItem{id}
{}

void Building::draw(QGraphicsItemGroup* group) const
{
    if (d_points.empty())
    {
        qDebug() << "No nodes available to draw the building.";
        return;
    }

    QPolygonF polygon;

    for(const QPointF& p: d_points)
    {
        polygon << p;
    }

    auto building = new QGraphicsPolygonItem(polygon, group);
//    building->setPolygon(polygon);
    QBrush brush{QColor(216, 208, 201)};
//    QBrush brush{QColor(0, 0, 0)};
    building->setBrush(brush);
    QPen pen{QColor(190, 190, 189), 1}; // Noir, épaisseur 1 pixel
    building->setPen(pen);
    group->addToGroup(building);
}
