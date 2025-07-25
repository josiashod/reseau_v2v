#ifndef PARK_H
#define PARK_H

#include "mapitem.h"
#include <QGraphicsObject>

class Park : public MapItem, public QGraphicsObject
{
public:
    Park(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPolygonF d_polygon;
};

#endif // PARK_H
