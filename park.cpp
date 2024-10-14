#include "park.h"

Park::Park(long long id): MapItem{id}
{}

void Park::draw(QGraphicsItemGroup* group, double scale_factor) const
{
    QPolygonF polygon;

    for(const Node& n: d_nodes)
    {
        polygon << n;
    }

    auto park = new QGraphicsPolygonItem();
    park->setPolygon(polygon);
    QBrush brush{QColor(141, 197, 108)};
    QPen pen{QColor(0, 0, 0), 1}; // Noir, épaisseur 1 pixel
    park->setPen(pen);
    park->setBrush(brush);
    qDebug() << "draw.";

    group->addToGroup(park);
}
