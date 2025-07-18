#ifndef HEXAGON_H
#define HEXAGON_H

#include <QGraphicsObject>

class Hexagon : public QGraphicsObject
{
    Q_OBJECT
public:
    Hexagon(QPolygonF hexagon, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPolygonF d_polygon;
};

#endif // HEXAGON_H
