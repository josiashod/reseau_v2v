#include "way.h"

#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPathItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QPolygonF>
#include <QPen>
#include <QPainter>
#include <QFont>

// Constructeur de la classe Way
Way::Way(long long id, const std::vector<QPointF>& points, QGraphicsItem* parent)
    : MapItem{id, points}, QGraphicsObject{parent}
{
    if(!d_points.empty())
    {
        for(const QPointF& p: d_points)
        {
            d_polygon << p;
        }
    }
}

QRectF Way::boundingRect() const
{
    return d_polygon.boundingRect();
}

QPainterPath Way::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

bool Way::isCarWay() const
{
    QString type = tag("highway");
    return type == "motorway" || type == "primary" || type == "secondary" ||
           type == "tertiary" || type == "residential" || type == "unclassified" ||
           type == "service";
}

void Way::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto originalPen    = painter->pen();

    QPen pen{originalPen};
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    // Récupérer le type de la route via le tag "highway"
    QString type = tag("highway");

    // Configuration du style de la route en fonction du type
    if (type == "footway" || type == "cycleway" || type == "steps") {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(1);
        pen.setColor(type == "cycleway" ? QColor{"#0080FF"} : QColor{"#FF7F00"});
    }
    else
    {
        pen.setWidth(3);
        if(isCarWay())
            pen.setColor(QColor{"#FED7A1"}); // Couleur par défaut pour les routes
        else if (type == "pedestrian") {
            pen.setColor(QColor{"#DDDDE9"});
            pen.setWidth(1);
        }
        else
            pen.setColor(QColor{"#F9C0CA"});
    }
    painter->setPen(pen);

    if(type != "bus_stop")
    {
        painter->drawPolyline(d_polygon);
    }

    painter->setPen(originalPen);
}
