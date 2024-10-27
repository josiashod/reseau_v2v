#include "building.h"

Building::Building(long long id): MapItem{id}
{}

void Building::draw(QGraphicsItemGroup* group, double scale_factor) const
{
    if (d_nodes.empty())
    {
        qDebug() << "No nodes available to draw the building.";
        return;
    }
    QPolygonF polygon;

    for(const Node& n: d_nodes)
    {
        polygon << n;
    }

    auto building = new QGraphicsPolygonItem();
    building->setPolygon(polygon);
    QBrush brush{QColor(216, 208, 201)};
//    QBrush brush{QColor(0, 0, 0)};
    building->setBrush(brush);
    QPen pen{QColor(190, 190, 189), 1}; // Noir, épaisseur 1 pixel
    building->setPen(pen);
    group->addToGroup(building);

    qDebug() << "drawing building: " << d_id;
}
