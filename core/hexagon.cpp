#include "hexagon.h"
#include "core/car.h"
#include <QPainter>
#include <QTimer>

size_t Hexagon::d_instance_counter = 0;

Hexagon::Hexagon(QPolygonF hexagon, QGraphicsItem *parent):
    QGraphicsObject{parent}, d_polygon{hexagon}, d_id{static_cast<int>(++d_instance_counter)}
{}

QRectF Hexagon::boundingRect() const
{
    return d_polygon.boundingRect();
}

QPainterPath Hexagon::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Hexagon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto originalPen    = painter->pen();
    QPen pen{QColor(0, 0, 0), 1};

    painter->setPen(pen);
    painter->drawPolygon(d_polygon);

    painter->setPen(originalPen);
}

void Hexagon::checkCarConnections()
{
    QList<QGraphicsItem*> collidingObjects = collidingItems(Qt::IntersectsItemShape);
    QVector<Car*> cars;

    for (auto* items : collidingObjects) {
        if(auto* car = dynamic_cast<Car*>(items))
        {
            cars.append(car);
        }
    }

    for (auto* car : cars) {
        car->fixedConnections();
    }

    for (int i = 0; i < cars.size(); ++i) {
        for (int j = 1; j < cars.size(); ++j) {
            if(cars[j] != cars[i])
            {
                cars[i]->updateConnectionWith(cars[j]);
                cars[j]->updateConnectionWith(cars[i]);
            }
        }
    }
}
