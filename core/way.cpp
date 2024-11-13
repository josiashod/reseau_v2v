#include "way.h"
#include "utils/dbmanager.h" // Inclusion nécessaire pour DBManager

#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsPathItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QPolygonF>
#include <QPen>
#include <QPainterPath>
#include <QFont>

// Constructeur de la classe Way
Way::Way(long long id) : MapItem{id}, d_id{id} {}

// Méthode pour dessiner la route sur la carte
void Way::draw(QGraphicsItemGroup* group) const {
    if (d_points.empty()) {
        qDebug() << "No nodes available to draw the way.";
        return;
    }

    QPolygonF polygon;
    QPen pen;
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    // Récupérer le type de la route via le tag "highway"
    QString type = tag("highway");

    // Configuration du style de la route en fonction du type
    if (type == "footway" || type == "cycleway") {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(1);
        pen.setColor(type == "cycleway" ? QColor{"#0080FF"} : QColor{"#FF7F00"});
    } else {
        pen.setWidth(4);
        pen.setColor(QColor{"#FED7A1"}); // Couleur par défaut pour les routes

        if (type == "pedestrian") {
            pen.setColor(QColor{"#DDDDE9"});
//            pen.setWidth();
        } else if (type == "platform") {
            return;
//            pen.setColor(QColor{"#B5B5B6"});
//            pen.setWidth(3);
        }
    }

    // Si le type est "bus_stop", dessiner des marqueurs de bus
    if (type == "bus_stop") {
        for(const QPointF& p: d_points)
        {
            QGraphicsTextItem* textItem = new QGraphicsTextItem("B");
            textItem->setPos(p);
            QFont font = textItem->font();
            font.setPointSize(3);
            textItem->setFont(font);
            group->addToGroup(textItem);
        }
    } else {
        // Ajouter tous les nœuds au polygone pour dessiner la route
        for(const QPointF& p: d_points)
        {
            polygon << p;
        }

        // Créer un chemin à partir du polygone
        QPainterPath path;
        path.addPolygon(polygon);
        QGraphicsPathItem* road = new QGraphicsPathItem(path);
        road->setPen(pen);
        group->addToGroup(road);
    }
}
