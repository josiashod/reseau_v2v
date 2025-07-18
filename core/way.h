#ifndef WAY_H
#define WAY_H

#include "mapitem.h"
#include <QGraphicsObject>

class Way: public MapItem, public QGraphicsObject
{
public:
   Way(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent = nullptr);

    /**
     * @brief isCarWay determine si un way est adapté au voiture
     * @return
     */
    bool isCarWay() const;

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    /**
     * @brief draw: dessine la route
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPolygonF d_polygon;
};

#endif // WAY_H
