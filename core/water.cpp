#include "water.h"

Water::Water(long long id): MapItem{id}
{}

void Water::draw(QGraphicsItemGroup* group, double scale_factor) const
{
    QPolygonF polygon;

    for(const Node& n: d_nodes)
    {
        polygon << n;
    }

    QPainterPath p;
    p.addPolygon(polygon);

    auto water = new QGraphicsPathItem(p);
    QBrush brush{QColor(0, 191, 255)};
    water->setBrush(brush);
    QPen pen{QColor(0, 191, 255), 1}; // Noir, épaisseur 1 pixel
    water->setPen(pen);

    group->addToGroup(water);

    qDebug() << "drawing water: " << d_id;
}
