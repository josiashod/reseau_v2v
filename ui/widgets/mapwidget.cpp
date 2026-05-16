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
        d_scene->setSceneRect(0, 0, width() * 4, height() * 4);
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

    d_parsedMap = OsmReader::parseMapFile(d_osmFilePath);
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

        if (wheelEvent->angleDelta().y() > 0 && d_scale_factor < 8.0)
        {
            // Zoom avant
            d_scale_factor *= scaleFactor;
            d_view->scale(scaleFactor, scaleFactor);
            // Compensation du décalage
            QPointF newMouseScenePos = d_view->mapToScene(wheelEvent->position().toPoint());
            d_view->translate(
                mouseScenePos.x() - newMouseScenePos.x(),
                mouseScenePos.y() - newMouseScenePos.y()
            );
        }
        else if (d_scale_factor > 0.7)
        {
            // Zoom arrière
            d_scale_factor *= (1.0 / scaleFactor);
            d_view->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }
    return QWidget::eventFilter(watched, event); // Passe les autres événements
}
void MapWidget::addCar(Car* car)
{
    d_carsLayer->addToGroup(car);
}

void MapWidget::setOsmFilePath(const QString& filePath)
{
    d_osmFilePath = filePath;
}

void MapWidget::loadOsmFile(const QString& filePath)
{
    d_osmFilePath = filePath;
    clearMapItems();

    if(d_graph)
        d_graph->clear();

    init();
}

void MapWidget::clearLayer(QGraphicsItemGroup* layer)
{
    const auto items = layer->childItems();
    for(auto* item: items) {
        layer->removeFromGroup(item);
        delete item;
    }
}

void MapWidget::clearMapItems()
{
    clearLayer(d_waterLayer);
    clearLayer(d_parkLayer);
    clearLayer(d_wayLayer);
    clearLayer(d_buildingLayer);
    clearLayer(d_meshLayer);
    clearLayer(d_carsLayer);
    d_hexagons.clear();
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
    QString log;

    if (d_parsedMap.bounds.valid) {
        const double minLat = d_parsedMap.bounds.minLat;
        const double maxLat = d_parsedMap.bounds.maxLat;
        const double minLon = d_parsedMap.bounds.minLon;
        const double maxLon = d_parsedMap.bounds.maxLon;

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
}

std::vector<QPointF> MapWidget::pointsFromFeature(const OsmFeature& feature)
{
    std::vector<QPointF> points;
    points.reserve(feature.nodeRefs.size());

    for(const long long nodeId: feature.nodeRefs) {
        auto nodeIt = d_parsedMap.nodes.find(nodeId);
        if(nodeIt == d_parsedMap.nodes.end())
            continue;

        auto coord = nodeIt->second;
        points.push_back(pairLatLonToXY(coord));
    }

    return points;
}

void MapWidget::initBuildings()
{
    for(const auto& building: d_parsedMap.buildings) {
        auto points = pointsFromFeature(building);
        if(points.empty())
            continue;

        auto b = new Building{building.id, points};
        QMetaObject::invokeMethod(this, [layer = d_buildingLayer, b]() {
            layer->addToGroup(b);
        }, Qt::QueuedConnection);
    }
}

void MapWidget::initParks()
{
    for(const auto& parkFeature: d_parsedMap.parks) {
        auto points = pointsFromFeature(parkFeature);
        if(points.empty())
            continue;

        auto park = new Park{parkFeature.id, points};
        QMetaObject::invokeMethod(this, [layer = d_parkLayer, park]() {
            layer->addToGroup(park);
        }, Qt::QueuedConnection);
    }
}

void MapWidget::initWaters()
{
    for(const auto& waterFeature: d_parsedMap.waters) {
        auto points = pointsFromFeature(waterFeature);
        if(points.empty())
            continue;

        auto water = new Water{waterFeature.id, points};
        QMetaObject::invokeMethod(this, [layer = d_waterLayer, water]() {
            layer->addToGroup(water);
        }, Qt::QueuedConnection);
    }
}

void MapWidget::initRoads()
{
    for(const auto& road: d_parsedMap.roads) {
        std::vector<QPointF> points;
        points.reserve(road.nodeRefs.size());

        Way defaultWay{road.id, {}};
        defaultWay.setTags(road.tags);

        osm::Node* start = nullptr;
        osm::Node* end = nullptr;

        for(const long long nodeId: road.nodeRefs) {
            auto nodeIt = d_parsedMap.nodes.find(nodeId);
            if(nodeIt == d_parsedMap.nodes.end())
                continue;

            auto coord = nodeIt->second;
            QPointF p = pairLatLonToXY(coord);
            points.push_back(p);

            if(d_graph && defaultWay.isCarWay()) {
                end = d_graph->addNode(nodeId, p.x(), p.y());
                if(start && end && !start->hasNeighbor(end))
                    d_graph->addEdge(start, end);

                start = end;
            }
        }

        if(points.empty())
            continue;

        auto w = new Way{road.id, points};
        w->setTags(road.tags);
        QMetaObject::invokeMethod(this, [layer = d_wayLayer, w]() {
            layer->addToGroup(w);
        }, Qt::QueuedConnection);
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
