#ifndef BUILDING_H
#define BUILDING_H

#include "mapitem.h"

class Building : public MapItem, public QGraphicsObject
{
public:
    Building(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    QPolygonF d_polygon;
};

#endif // BUILDING_H
