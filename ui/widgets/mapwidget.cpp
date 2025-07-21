#include "mapwidget.h"
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QMenu>

#include "core/graph.h"
#include "core/way.h"
#include "core/building.h"
#include "core/park.h"
#include "core/car.h"
#include "./utils/dbmanager.h"
#include "core/hexagon.h"
#include "core/water.h"
#include "ui/widgets/logwidget.h"


// CRÉATION DE L'INTERFACE

MapWidget::MapWidget(QWidget *parent, osm::Graph* graph)
    : QWidget{parent}, d_graph{graph}
{
    creerInterface();
}

MapWidget::~MapWidget()
{
    for (auto *item : d_scene->items()) {
        delete item; // Libère chaque élément
    }
    d_scene->clear();
    delete d_scene;
}

void MapWidget::creerInterface()
{
    // Add d_view to layout so it fills the widget
    auto layout = new QVBoxLayout{this};

    d_view = new QGraphicsView{};
    d_view->setMouseTracking(true);
    d_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_view->setDragMode(QGraphicsView::ScrollHandDrag);
    d_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    d_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    d_view->setRenderHint(QPainter::Antialiasing, true);
    d_view->viewport()->installEventFilter(this);

    d_scene = new QGraphicsScene(this);
    d_scene->setBackgroundBrush(QColor("#F2EFE9")); // Gris clair
    d_view->setScene(d_scene);

    d_waterLayer = new QGraphicsItemGroup();
    d_waterLayer->setVisible(true);
    d_scene->addItem(d_waterLayer);

    d_parkLayer = new QGraphicsItemGroup();
    d_parkLayer->setVisible(d_showPark);
    d_parkLayer->setAcceptedMouseButtons(Qt::NoButton);
    d_scene->addItem(d_parkLayer);

    d_wayLayer = new QGraphicsItemGroup();
    d_wayLayer->setVisible(d_showWay);
    d_wayLayer->setAcceptedMouseButtons(Qt::NoButton);
    d_scene->addItem(d_wayLayer);

    d_buildingLayer = new QGraphicsItemGroup();
    d_buildingLayer->setVisible(d_showBuilding);
    d_buildingLayer->setAcceptedMouseButtons(Qt::NoButton);
    d_scene->addItem(d_buildingLayer);

    d_meshLayer = new QGraphicsItemGroup();
    d_meshLayer->setVisible(d_showMesh);
    d_meshLayer->setAcceptedMouseButtons(Qt::NoButton);
    d_scene->addItem(d_meshLayer);

    d_carsLayer = new QGraphicsItemGroup();
    d_carsLayer->setVisible(d_showCar);
    d_carsLayer->setHandlesChildEvents(false);
    d_scene->addItem(d_carsLayer);

    layout->addWidget(d_view);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    if(d_default_scene_rect.size() == QSize(0,0))
    {
        d_scene->setSceneRect(0, 0, width() * 2.5, height() * 2.5);
        d_view->fitInView(d_scene->sceneRect(), Qt::KeepAspectRatio);
        d_view->setAlignment(Qt::AlignCenter);
        d_default_scene_rect = d_scene->sceneRect();
        d_view->scale(2, 2);
        init();
    }
}


void MapWidget::init()
{
    // Mettre à jour la taille de la scène lors du redimensionnement de la vue
    emit isLoaded(false);

    initBounds();
    initMeshs();

    auto cleanWatcher = [](QFutureWatcher<void>* watcher) {
        watcher->deleteLater();
    };

    auto roadWatcher = new QFutureWatcher<void>(this);
    auto defaultWatcher = new QFutureWatcher<void>(this);
    auto buildingWatcher = new QFutureWatcher<void>(this);

    connect(roadWatcher, &QFutureWatcher<void>::finished, this, [this, cleanWatcher, roadWatcher](){
        emit isLoaded(true);
        cleanWatcher(roadWatcher);
    });
    connect(defaultWatcher, &QFutureWatcher<void>::finished, this, [ cleanWatcher, defaultWatcher](){
        cleanWatcher(defaultWatcher);
    });
    connect(buildingWatcher, &QFutureWatcher<void>::finished, this, [cleanWatcher, buildingWatcher](){
       cleanWatcher(buildingWatcher);
    });

    // Associe le watcher au future
    defaultWatcher->setFuture(QtConcurrent::run([this]() {
        initParks();
        initWaters();
    }));
    roadWatcher->setFuture(QtConcurrent::run([this]() {
        initRoads();
    }));
    buildingWatcher->setFuture(QtConcurrent::run([this]() {
       initBuildings();
    }));
}

bool MapWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d_view->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);

        const double scaleFactor = 1.1;
        QPointF mouseScenePos = d_view->mapToScene(wheelEvent->position().toPoint());

        if (wheelEvent->angleDelta().y() > 0)
        {
            // Zoom avant
            d_scale_factor *= scaleFactor;
            d_view->scale(scaleFactor, scaleFactor);
        }
        else if (d_scale_factor > 0.7)
        {
            // Zoom arrière
            d_scale_factor *= (1.0 / scaleFactor);
            d_view->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }

        // Compensation du décalage
        QPointF newMouseScenePos = d_view->mapToScene(wheelEvent->position().toPoint());
        d_view->translate(
            mouseScenePos.x() - newMouseScenePos.x(),
            mouseScenePos.y() - newMouseScenePos.y()
        );
    }
    return QWidget::eventFilter(watched, event); // Passe les autres événements
}
void MapWidget::addCar(Car* car)
{
    d_carsLayer->addToGroup(car);
}

QPointF MapWidget::pairLatLonToXY(std::pair<double, double>& coord)
{
    return latLonToXY(coord.first, coord.second);
}

QPointF MapWidget::latLonToXY(double lon, double lat) {
    double width = d_default_scene_rect.size().width();
    double height = d_default_scene_rect.size().height();

    double x = (lon - d_minCoord.first) / (d_maxCoord.first - d_minCoord.first) * width;
    double y = height - (lat - d_minCoord.second) / (d_maxCoord.second - d_minCoord.second) * height;
    return {x, y};
}


void MapWidget::initBounds()
{
    // auto d_dbmanager = DBManager();
    QSqlQuery query = DBManager::instance().getBounds();
    double minLat = 0.0, maxLat = 0.0, minLon = 0.0, maxLon = 0.0;
    bool success = false;
    QString log;

    success = query.exec();

    if (success && query.next()) {
        // je récupère les bornes de la table nodes
        minLat = query.value("minlat").toDouble();
        maxLat = query.value("maxlat").toDouble();
        minLon = query.value("minlon").toDouble();
        maxLon = query.value("maxlon").toDouble();

        d_maxCoord = std::make_pair(maxLon, maxLat);
        d_minCoord = std::make_pair(minLon, minLat);

        // Vérification des bornes projetées
        log = "[INFO] Max Coord: lon -> " + QString::number(maxLon, 'f', 5) + ", lat -> " + QString::number(maxLat, 'f', 5) + ".";
        LogWidget::addLog(log);

        qDebug() << log;
        log = "[INFO] Min Coord: lon -> " + QString::number(minLon, 'f', 5) + ", lat -> " + QString::number(minLat, 'f', 5) + ".";
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
        LogWidget::addLog(log);
        qDebug() << log;

    } else {
        log = "[ERREUR] Impossible de récupérer les longitude et latitudes min/max.";
        LogWidget::addLog(log, LogWidget::DANGER);
        qDebug() << log;
    }
    query.finish();
    // DBManager::instance().close();
}

void MapWidget::initBuildings()
{
    // uncomment this if you want to load the building
    return;
    auto query = DBManager::instance().getBuildings();
    bool success = false;

    success = query.exec();
    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            std::vector<QPointF> points;

            auto q = DBManager::instance().getWayNodes(id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
                    double lat = 0.0, lon = 0.0;

                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    coord = std::make_pair(lon, lat);
                    points.push_back(pairLatLonToXY(coord));

                }
            }
            auto b = new Building{id, points};
            QMetaObject::invokeMethod(this, [layer = d_buildingLayer, b]() {
                layer->addToGroup(b);
            }, Qt::QueuedConnection);
        }
        query.finish();
    }
    else
    {
        qWarning() << "Erreur lors de la lecture de la  base de donnée";
    }

}

void MapWidget::initParks()
{
    auto query = DBManager::instance().getParks();
    bool success = false;

    success = query.exec();
    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();
            std::vector<QPointF> points;

            auto q = DBManager::instance().getWayNodes(id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
                    double lat = 0.0, lon = 0.0;

                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();
                    coord = std::make_pair(lon, lat);
                    // QPointF p = ;
                    points.push_back(pairLatLonToXY(coord));
                }
            }
            auto park = new Park{id, points};
            QMetaObject::invokeMethod(this, [layer = d_parkLayer, park]() {
                layer->addToGroup(park);
            }, Qt::QueuedConnection);
        }
        query.finish();
    }
    else
    {
        qWarning() << "Erreur lors de la lecture de la  base de donnée";
    }
}

void MapWidget::initWaters()
{
    auto query = DBManager::instance().getWaters();
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();
            std::vector<QPointF> points;

            auto q = DBManager::instance().getWayNodes(id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
                    double lat = 0.0, lon = 0.0;

                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();
                    coord = std::make_pair(lon, lat);
                    points.push_back(pairLatLonToXY(coord));
                }
            }

            auto water = new Water{id, points};
            QMetaObject::invokeMethod(this, [layer = d_waterLayer, water]() {
                layer->addToGroup(water);
            }, Qt::QueuedConnection);
        }
        query.finish();
    }
}

void MapWidget::initRoads()
{
    auto query = DBManager::instance().getRoads();
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();
            std::vector<QPointF> points;
            std::map<QString, QString> tags;
            
            QString key = query.value(4).toString();
            QString value = query.value(5).toString();
            
            tags[key] = value;

            Way defaultWay{0, {}};
            defaultWay.addTag(key, value);

            auto q = DBManager::instance().getWayNodes(id);
            success = q.exec();
            if(success)
            {
                long long id;
                double lat = 0.0, lon = 0.0;
                QPointF p;
                osm::Node* start    = nullptr;
                osm::Node* end      = nullptr;

                if (d_graph &&  defaultWay.isCarWay())
                {
                    if(q.next())
                    {
                        id = q.value(0).toString().toLongLong();
                        lat = q.value(1).toString().toDouble();
                        lon = q.value(2).toString().toDouble();
                        std::pair<double, double> c{lon, lat};
                        p = pairLatLonToXY(c);
                        points.push_back(p);
                        start = d_graph->addNode(id, p.x(), p.y());
                    }
                }
                while(q.next())
                {

                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();
                    std::pair<double, double> coord{lon, lat};
                    p = pairLatLonToXY(coord);
                    points.push_back(p);

                    // Ajouter un nœud au graphe et connecter l'arête
                    if (start) {
                        end = d_graph->addNode(id, p.x(), p.y());
                        if (start && end) {
                            // on verifie si l'arret n'existe pas dejà
                            if (!start->hasNeighbor(end)) {
                                d_graph->addEdge(start, end);
                            }
                        }
                        start = end;
                    }
                }
                auto w = new Way{id, points};
                w->setTags(tags);
                QMetaObject::invokeMethod(this, [layer = d_wayLayer, w]() {
                    layer->addToGroup(w);
                }, Qt::QueuedConnection);
            }

        }
        query.finish();
    }

}

void MapWidget::initMeshs()
{
    qreal hexRadius = 250.0;  // Ajuster la taille au besoin
    qreal dx = 1.5 * hexRadius;  // Décalage horizontal (distance entre centres des hexagones)
    qreal dy = sqrt(3) * hexRadius;  // Décalage vertical (distance entre centres des hexagones)


    QPointF topLeft = d_default_scene_rect.topLeft();    // Coin supérieur gauche
    QPointF bottomRight = d_default_scene_rect.bottomRight();  // Coin inférieur droit

    qreal startX = topLeft.x();
    qreal startY = topLeft.y();
    qreal endX = bottomRight.x();
    qreal endY = bottomRight.y();

    int rows = std::ceil((endY - startY) / dy);  // Nombre de rangées
    int cols = std::ceil((endX - startX) / dx);  // Nombre de colonnes


    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            qreal x = startX + j * dx;
            qreal y = startY + i * dy;

            // Décalage pour les rangées impaires
            if (j % 2 == 1) {
                y += dy / 2;
            }

            QPolygonF hexagon;

            // Créer les sommets de l'hexagone
            for (int k = 0; k < 6; ++k) {
                qreal angle = 2 * M_PI * k / 6.0;
                qreal vertexX = x + hexRadius * cos(angle);
                qreal vertexY = y + hexRadius * sin(angle);
                hexagon << QPointF(vertexX, vertexY);
            }

            auto* hex = new Hexagon{hexagon, d_meshLayer};
            d_meshLayer->addToGroup(hex);
            d_hexagons.push_back(hex);
        }
    }
}

void MapWidget::checkCarsConnections()
{
    for (auto& hex: d_hexagons) {
        hex->checkCarConnections();
    }
}

void MapWidget::setShowPark(bool show)
{
    d_showPark = show;
    d_parkLayer->setVisible(d_showPark);
}

void MapWidget::setShowBuilding(bool show)
{
    d_showBuilding = show;
    d_buildingLayer->setVisible(d_showBuilding);
}

void MapWidget::setShowRoad(bool show)
{
    d_showWay = show;
    d_wayLayer->setVisible(d_showWay);
}

void MapWidget::setShowHex(bool show)
{
    d_showMesh = show;
    d_meshLayer->setVisible(d_showMesh);
}
