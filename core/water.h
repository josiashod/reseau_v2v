#ifndef WATER_H
#define WATER_H

#include "mapitem.h"

class Water : public MapItem, public QGraphicsObject
{
public:
    Water(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPolygonF d_polygon;
};

#endif // WATER_H
