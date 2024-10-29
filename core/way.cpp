#include "way.h"

#include <QGraphicsLineItem>
#include <QFont>

Way::Way(long long id): MapItem{id}
{}

void Way::draw(QGraphicsItemGroup* group, double scale_factor) const
{
    if (d_nodes.empty())
    {
        qDebug() << "No nodes available to draw the building.";
        return;
    }

    QPolygonF polygon;

    QPen pen{};
    pen.setJoinStyle(Qt::RoundJoin);

    QString type = tag("highway");

    if(type == "footway" || type == "cycleway")
    {
        pen.setStyle(Qt::DotLine);
        if(type == "cycleway")
            pen.setColor(QColor{"#0080FF"});
        else
            pen.setColor(QColor{"#FF7F00"});
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
    }
    else
    {
        pen.setWidth(11);
        pen.setColor(QColor{"#FED7A1"});

//        if (type == "motorway")
//        {
//            pen.setColor(QColor{"#E990A0"});
//        }
//        if (type == "trunk")
//        {
//            pen.setColor(QColor{"#FBC0AC"});
//        }
//        if (type == "primary")
//        {
//            pen.setColor(QColor{"#FED7A1"});
//        }
//        if (type == "secondary")
//        {
//            pen.setColor(QColor{"#F6F6BB"});
//        }
//        if (type == "tertiary")
//        {
//            pen.setColor(QColor{"#FEFEFE"});
//            pen.setWidth(10);
//        }
//        if (type == "residential")
//        {
//            pen.setColor(QColor{"#FEFEFE"});
//            pen.setWidth(10);
//        }
//        if (type == "unclassified")
//        {
//            pen.setColor(QColor{"#FEFEFE"});
//            pen.setWidth(10);
//        }
//        if (type == "service")
//        {
//            pen.setColor(QColor{"#FEFEFE"});
//            pen.setWidth(10);
//        }
        if (type == "pedestrian")
        {
            pen.setColor(QColor{"#DDDDE9"});
            pen.setWidth(6);
        }
//        if(type == "living_street")
//        {
//            pen.setColor(QColor{"#ECEEED"});
//            pen.setWidth(3);
//        }
        if(type == "platform")
        {
            pen.setColor(QColor{"#B5B5B6"});
            pen.setWidth(3);
        }
    }

    if(type == "bus_stop")
    {
        for(const Node& n: d_nodes)
        {
            QGraphicsTextItem *textItem = new QGraphicsTextItem("B");
            textItem->setPos(n);
            QFont font = textItem->font();
            font.setPointSize(3 * scale_factor);  // Ajuster la taille de la police
            textItem->setFont(font);

            group->addToGroup(textItem);
        }
    }
    else
    {
        for(const Node& n: d_nodes)
        {
            polygon << n;
        }

        QPainterPath p;
        p.addPolygon(polygon);
        auto road = new QGraphicsPathItem(p);
        road->setPen(pen);
        group->addToGroup(road);
    }

//    for(int i = 1; i < d_nodes.size(); ++i)
//    {

//        QLineF line{d_nodes[i - 1], d_nodes[i]};

//        auto road = new QGraphicsLineItem(line);
//        road->setPen(pen);
//        group->addToGroup(road);
//    }
}

unsigned int Way::nodeSize() const {
    return d_nodes.size();
}
