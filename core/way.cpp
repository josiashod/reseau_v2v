#include "way.h"
#include "utils/dbmanager.h" // Inclusion nécessaire pour DBManager
#include "node.h"

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
void Way::draw(QGraphicsItemGroup* group, double scale_factor) const {
    if (d_nodes.empty()) {
        qDebug() << "No nodes available to draw the way.";
        return;
    }

    QPolygonF polygon;
    QPen pen;
    pen.setJoinStyle(Qt::RoundJoin);

    // Récupérer le type de la route via le tag "highway"
    QString type = tag("highway");

    // Configuration du style de la route en fonction du type
    if (type == "footway" || type == "cycleway") {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        pen.setColor(type == "cycleway" ? QColor{"#0080FF"} : QColor{"#FF7F00"});
    } else {
        pen.setWidth(11 / scale_factor);
        pen.setColor(QColor{"#FED7A1"}); // Couleur par défaut pour les routes

        if (type == "pedestrian") {
            pen.setColor(QColor{"#DDDDE9"});
            pen.setWidth(6);
        } else if (type == "platform") {
            pen.setColor(QColor{"#B5B5B6"});
            pen.setWidth(3);
        }
    }

    // Si le type est "bus_stop", dessiner des marqueurs de bus
    if (type == "bus_stop") {
        for (const Node& n : d_nodes) {
            QGraphicsTextItem* textItem = new QGraphicsTextItem("B");
            textItem->setPos(n);
            QFont font = textItem->font();
            font.setPointSize(3 * scale_factor);
            textItem->setFont(font);
            group->addToGroup(textItem);
        }
    } else {
        // Ajouter tous les nœuds au polygone pour dessiner la route
        for (const Node& n : d_nodes) {
            polygon << n;
        }

        // Créer un chemin à partir du polygone
        QPainterPath path;
        path.addPolygon(polygon);
        QGraphicsPathItem* road = new QGraphicsPathItem(path);
        road->setPen(pen);
        group->addToGroup(road);
    }
}

// Méthode pour trouver un Way par son ID dans la base de données
Way* Way::findWayById(long long id) {
    auto db = DBManager::getInstance();
    QSqlQuery query(db->getDatabase());

    // Récupérer le way par son ID
    query.prepare("SELECT * FROM ways WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec() || !query.next()) {
        qDebug() << "Erreur lors de la récupération du way :" << query.lastError().text();
        return nullptr;
    }

    // Créer un nouvel objet Way
    Way* way = new Way(id);

    // Récupérer tous les nœuds associés au way
    QSqlQuery nodesQuery = db->getWayNodes(db->getDatabase(), id);
    if (nodesQuery.exec()) {
        while (nodesQuery.next()) {
            long long nodeId = nodesQuery.value(0).toLongLong();
            double lat = nodesQuery.value(1).toDouble();
            double lon = nodesQuery.value(2).toDouble();

            // Ajouter le nœud au Way
            Node node(nodeId, {lon, lat});
            way->addNode(node);
        }
    } else {
        qDebug() << "Erreur lors de la récupération des nœuds pour le way :" << nodesQuery.lastError().text();
    }

    return way;
}

// Retourner tous les nœuds associés au way
QVector<Node> Way::nodes() const {
    return d_nodes;
}

// Ajouter un nœud au Way
void Way::addNode(const Node& node) {
    d_nodes.append(node);
}

// Retourner la taille des nœuds
unsigned int Way::nodeSize() const {
    return d_nodes.size();
}
