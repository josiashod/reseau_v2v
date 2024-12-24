#include "mapwidget.h"
#include <QGraphicsItemAnimation>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include "logwidget.h"
#include "../../core/graph.h"


// CRÉATION DE L'INTERFACE

MapWidget::MapWidget(QWidget *parent, osm::Graph* graph)
    : QGraphicsView{parent}, d_graph{graph}
{
    d_dbmanager = DBManager::getInstance();
    creerInterface();
//    initMeshs();
    initBounds();
}

MapWidget::~MapWidget()
{
    DBManager::closeDatabase();
    DBManager::destroyInstance();
    d_scene->clear();
}

void MapWidget::creerInterface()
{
//    QVBoxLayout *layout = new QVBoxLayout(this);
//    layout->setContentsMargins(0, 0, 0, 0);
//    setLayout(layout);

//    d_view = new QGraphicsView{this};

//    QTransform transform;
//    transform.shear(0.4, 0); // Cisaillement horizontal pour simuler l'inclinaison
//    transform.scale(1, 0.4); // Réduit la hauteur pour un effet isométrique
//    setTransform(transform);
//    rotate(-10);

    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag);

    d_scene = new QGraphicsScene(this);
    d_scene->setBackgroundBrush(QColor("#F2EFE9")); // Gris clair
    setScene(d_scene);
    show();

//    d_waterLayer = new QGraphicsItemGroup();
//    d_waterLayer->setVisible(d_showWater);
//    d_scene->addItem(d_waterLayer);

    d_parkLayer = new QGraphicsItemGroup();
    d_parkLayer->setVisible(d_showPark);
    d_scene->addItem(d_parkLayer);

    d_wayLayer = new QGraphicsItemGroup();
    d_wayLayer->setVisible(d_showWay);
    d_scene->addItem(d_wayLayer);

    d_buildingLayer = new QGraphicsItemGroup();
    d_buildingLayer->setVisible(d_showBuilding);
    d_scene->addItem(d_buildingLayer);

    d_freqCarsLayer = new QGraphicsItemGroup();
    d_freqCarsLayer->setVisible(d_showCarFreq);
    d_scene->addItem(d_freqCarsLayer);

    d_carsLayer = new QGraphicsItemGroup();
    d_carsLayer->setVisible(d_showCar);
    d_scene->addItem(d_carsLayer);

    d_meshLayer = new QGraphicsItemGroup();
    d_meshLayer->setVisible(d_showMesh);
    d_scene->addItem(d_meshLayer);

//    layout->addWidget(d_view);

    connect(this, &MapWidget::buildingsDataReady, this, &MapWidget::drawBuildingLayer);
    connect(this, &MapWidget::parksDataReady, this, &MapWidget::drawParkLayer);
//    connect(this, &MapWidget::watersDataReady, this, &MapWidget::drawWaterLayer);
    connect(this, &MapWidget::roadsDataReady, this, &MapWidget::drawRoadLayer);
}

void MapWidget::drawBuildingLayer(const QVector<Building>& buildings)
{
    QString log;
    for (const Building& b : buildings)
    {
        b.draw(d_buildingLayer); // Dessine dans le thread principal
        log = QString("[INFO] Drawing building n°: %1.").arg(b.id());
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
            qDebug() << log;
    }
}

/*void MapWidget::drawWaterLayer(const QVector<Water>& waters)
{
    QString log;
    for (const Water& w : waters)
    {
        w.draw(d_waterLayer); // Dessine dans le thread principal
        log = QString("[INFO] Drawing water n°: %1.").arg(w.id());
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
            qDebug() << log;
    }
}*/

void MapWidget::drawParkLayer(const QVector<Park>& parks)
{
    QString log;
    for (const Park& p : parks)
    {
        p.draw(d_parkLayer); // Dessine dans le thread principal
        log = QString("[INFO] Drawing park n°: %1.").arg(p.id());
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
            qDebug() << log;
    }
}

void MapWidget::drawRoadLayer(const QVector<Way>& ways)
{
    QString log;
    for (const Way& w : ways)
    {
        w.draw(d_wayLayer); // Dessine dans le thread principal
        log = QString("[INFO] Drawing road n°: %1.").arg(w.id());
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
//            qDebug() << log;
    }
}

void MapWidget::drawMeshLayer()
{
    for (QPolygonF& hexagon : d_meshs)
    {
        // Créer un nouvel élément texte
        QGraphicsPolygonItem *polygon = new QGraphicsPolygonItem();
        polygon->setPolygon(hexagon);

        d_meshLayer->addToGroup(polygon);
    }
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    // Mettre à jour la taille de la scène lors du redimensionnement de la vue
    QGraphicsView::resizeEvent(event);

    if(!d_elementsHasBeenLoaded)
    {
        emit isLoading(d_elementsHasBeenLoaded);

        setRenderHint(QPainter::Antialiasing);
        //initMeshs();
        drawMeshLayer();
//        observation_point = QPointF(event->size().width() / 2, event->size().height() / 2);
    //    d_scene->setSceneRect(0, 0, event->size().width() * 2, event->size().height() * 2);
        fitInView(d_scene->sceneRect(), Qt::KeepAspectRatio);
        // Lancer la fonction longue en asynchrone
        QFuture<void> future = QtConcurrent::run([this]() {
            this->initParks();
            this->initRoads();
//            this->initBuildings();
        });

        // Configurer la vue (taille et centrage)
        setAlignment(Qt::AlignCenter);

        auto watcher = new QFutureWatcher<void>(this);
        connect(watcher, &QFutureWatcher<void>::finished, this, &MapWidget::isLoadingFinished);

        // Associe le watcher au future
        watcher->setFuture(future);
    }
}

void MapWidget::isLoadingFinished()
{
    d_elementsHasBeenLoaded = true;
    emit isLoaded(d_elementsHasBeenLoaded);
}

void MapWidget::wheelEvent(QWheelEvent *event)
{
    // Zoomer avec la molette de la souris
    const double scaleFactor = 1.1;  // Facteur de zoom

    if (event->angleDelta().y() > 0) {
        // Zoom avant
        d_scale_factor *= scaleFactor;
        scale(scaleFactor, scaleFactor);
    } else {
        // Zoom arrière
        if(d_scale_factor > 1.0)
        {
            d_scale_factor *= (1.0 / scaleFactor);
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }
}

void MapWidget::addCarImage(QGraphicsPixmapItem* car)
{
    d_carsLayer->addToGroup(car);
}

void MapWidget::addCarEllipse(QGraphicsEllipseItem* ellipse)
{
    d_freqCarsLayer->addToGroup(ellipse);
}

/*std::pair<double, double> MapWidget::lambert93(double longitude, double latitude)
{
    // Créer un contexte PROJ
    PJ_CONTEXT *ctx = proj_context_create();
    if (ctx == nullptr) {
        throw std::runtime_error("Failed to create PROJ context.");
    }

    // Définir les systèmes de coordonnées avec des chaînes PROJ
    const char* wgs84 = "+proj=longlat +datum=WGS84 +no_defs";
    const char* lambert93 = "+proj=lcc +lat_1=49 +lat_2=44 +lat_0=46.5 +lon_0=3 +x_0=700000 +y_0=6600000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs";

    // Créer la transformation CRS
    PJ *P = proj_create_crs_to_crs(ctx, wgs84, lambert93, nullptr);
    if (P == nullptr) {
        proj_context_destroy(ctx);
        throw std::runtime_error("Failed to create CRS transformation.");
    }

    // Convertir les coordonnées
    PJ_COORD coord = proj_coord(longitude, latitude, 0, 0);
    PJ_COORD result = proj_trans(P, PJ_FWD, coord);

    // Vérifier si la transformation a échoué
    if (result.xyzt.x == HUGE_VAL || result.xyzt.y == HUGE_VAL) {
        proj_destroy(P);
        proj_context_destroy(ctx);
        throw std::runtime_error("Transformation failed.");
    }

    // Libérer les ressources
    proj_destroy(P);
    proj_context_destroy(ctx);

    return {result.xyzt.x, result.xyzt.y}; // Retourner les coordonnées Lambert93
}*/

QPointF MapWidget::pairLatLonToXY(std::pair<double, double>& coord)
{
    return latLonToXY(coord.first, coord.second);
}

QPointF MapWidget::latLonToXY(double lon, double lat) {
    double width = this->width();
    double height = this->height();

    double x = (lon - d_minCoord.first) / (d_maxCoord.first - d_minCoord.first) * width;
    double y = height - (lat - d_minCoord.second) / (d_maxCoord.second - d_minCoord.second) * height;

//    return {x, y * d_perspective_offset};
    return {x, y};
}


void MapWidget::initBounds()
{
    QSqlQuery query = d_dbmanager->getBounds(d_dbmanager->getDatabase());
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

        // Applique la transformation Lambert93 aux bornes
//        d_maxCoord = lambert93(maxLon, maxLat);
//        d_minCoord = lambert93(minLon, minLat);
        d_maxCoord = std::make_pair(maxLon, maxLat);
        d_minCoord = std::make_pair(minLon, minLat);

        // Vérification des bornes projetées
        log = "[INFO] Max Coord (Lambert93): lon -> " + QString::number(maxLon, 'f', 5) + ", lat -> " + QString::number(maxLat, 'f', 5) + ".";
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
            qDebug() << log;
        log = "[INFO] Min Coord (Lambert93): lon -> " + QString::number(minLon, 'f', 5) + ", lat -> " + QString::number(minLat, 'f', 5) + ".";
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
            qDebug() << log;

    } else {
        log = "[ERREUR] Impossible de récupérer les longitude et latitudes min/max.";
        // if(d_logger)
        //     d_logger->addLog(log, LogWidget::DANGER);
        // else
            qDebug() << log;
    }
    query.clear();
}

void MapWidget::initBuildings()
{
    QVector<Building> buildings;
    auto query = d_dbmanager->getBuildings(d_dbmanager->getDatabase());
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Building b{id};

            auto q = d_dbmanager->getWayNodes(d_dbmanager->getDatabase(), id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
                    double lat = 0.0, lon = 0.0;

                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    //            coord = lambert93(lon, lat);
                    coord = std::make_pair(lon, lat);
                    QPointF p = pairLatLonToXY(coord);
                    b.addPoint(p);
//                    qDebug() << QString("[SUCCESS] Building n°: %1.").arg(id);
                }
            }
            buildings.push_back(b);
            // if(d_logger)
            //     d_logger->addLog(QString("[SUCCESS] Building n°: %1.").arg(id), LogWidget::SUCCESS);
        }
        query.clear();
    }
    // else
    // {
    //     if(d_logger)
    //         d_logger->addLog("[ERREUR] Un problème est survenu lors de la récupération des buildings", LogWidget::DANGER);
    // }

    // if(d_logger)
    //     d_logger->addLog("[INFO] Emission des buildings pour affichage.");
    emit buildingsDataReady(buildings);
}

void MapWidget::initParks()
{
    QVector<Park> parks;
    auto query = d_dbmanager->getParks(d_dbmanager->getDatabase());
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Park park{id};

            auto q = d_dbmanager->getWayNodes(d_dbmanager->getDatabase(), id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
//                    long long id;
                    double lat = 0.0, lon = 0.0;

//                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    //            coord = lambert93(lon, lat);
                    coord = std::make_pair(lon, lat);
                    QPointF p = pairLatLonToXY(coord);
                    park.addPoint(p);
                }
            }
            parks.push_back(park);
            // if(d_logger)
            // d_logger->addLog(QString("[SUCCESS] Park n°: %1.").arg(id), LogWidget::SUCCESS);
        }
        query.clear();
    }
    // else
    // {
    //     if(d_logger)
    //     d_logger->addLog("[ERREUR] Un problème est survenu lors de la récupération des parks", LogWidget::DANGER);
    // }
    // if(d_logger)
    // d_logger->addLog("[INFO] Emission des parks pour affichage.");
    emit parksDataReady(parks);
}

/*void MapWidget::initWaters()
{
    QVector<Water> waters;
    auto query = d_dbmanager->getWaters(d_dbmanager->getDatabase());
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Water w{id};

            auto q = d_dbmanager->getWayNodes(d_dbmanager->getDatabase(), id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
//                    long long id;
                    double lat = 0.0, lon = 0.0;

//                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    //            coord = lambert93(lon, lat);
                    coord = std::make_pair(lon, lat);
                    QPointF p = pairLatLonToXY(coord);
                    w.addPoint(p);
                }
            }
            waters.push_back(w);
            // if(d_logger)
            // d_logger->addLog(QString("[SUCCESS] Water n°: %1.").arg(id), LogWidget::SUCCESS);
        }
        query.clear();
    }
    // else
    // {
    //     d_logger->addLog("[ERREUR] Un problème est survenu lors de la récupération des waters", LogWidget::DANGER);
    // }
    // d_logger->addLog("[INFO] Emission des waters pour affichage.");
    emit watersDataReady(waters);
}*/

void MapWidget::initRoads()
{
    QVector<Way> ways;
    auto query = d_dbmanager->getRoads(d_dbmanager->getDatabase());
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Way w{id};
            QString key = query.value(3).toString();
            QString value = query.value(4).toString();
            w.addTag(key, value);

            auto q = d_dbmanager->getWayNodes(d_dbmanager->getDatabase(), id);
            success = q.exec();
            if(success)
            {
                long long id;
                double lat = 0.0, lon = 0.0;
                QPointF p;
                osm::Node* start    = nullptr;
                osm::Node* end      = nullptr;

                if (d_graph &&  w.isCarWay())
                {
                    if(q.next())
                    {
                        id = q.value(0).toString().toLongLong();
                        lat = q.value(1).toString().toDouble();
                        lon = q.value(2).toString().toDouble();
                        std::pair<double, double> c{lon, lat};
                        p = pairLatLonToXY(c);
                        w.addPoint(p);
                        start = d_graph->addNode(id, p.x(), p.y());
                    }
                }
                while(q.next())
                {

                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    //            coord = lambert93(lon, lat);
                    std::pair<double, double> coord{lon, lat};
                    p = pairLatLonToXY(coord);
                    w.addPoint(p);

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
                    qDebug() << QString("[SUCCESS] Road n°: %1.").arg(id);
                }
                ways.push_back(w);
            }
            // if(d_logger)
            // d_logger->addLog(QString("[SUCCESS] Road n°: %1.").arg(id), LogWidget::SUCCESS);
        }
        query.clear();
    }
    // else
    // {
    //     if(d_logger)
    //         d_logger->addLog("[ERREUR] Un problème est survenu lors de la récupération des roads", LogWidget::DANGER);
    // }

    // if(d_logger)
    //     d_logger->addLog("[INFO] Emission des roads pour affichage.");
    emit roadsDataReady(ways);
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

void MapWidget::setShowFreq(bool show)
{
    d_showCarFreq = show;
    d_freqCarsLayer->setVisible(show);
}

//void MapWidget::loadCars()
//{
//    for(int i = 0; i < 1; i++)
//    {
////        osm::Node*  n = d_graph.getRandomNode();

////        if(n)
////        {
////            d_cars.push_back(std::make_unique<Car>(Car{n}));
////            d_freqCarsLayer->addToGroup(d_cars.back()->ellipse());
////            d_carsLayer->addToGroup(d_cars.back()->pixmap());
////        }
//    }
//}

void MapWidget::initMeshs()
{
    qreal hexRadius = 70.0;  // Ajuster la taille au besoin
    qreal dx = 1.5 * hexRadius;  // Décalage horizontal (distance entre centres des hexagones)
    qreal dy = sqrt(3) * hexRadius;  // Décalage vertical (distance entre centres des hexagones)

    int rows = height() / dy;  // Calcul du nombre de rangées en fonction de la hauteur de la scène
    int cols = width() / dx;   // Calcul du nombre de colonnes en fonction de la largeur de la scène

    // Ajuster légèrement la taille des hexagones pour combler toute la scène
    qreal adjustedHexRadius = std::min(width() / (cols * 1.5), height() / (rows * sqrt(3)));
    dx = 1.5 * adjustedHexRadius;
    dy = sqrt(3) * adjustedHexRadius;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            qreal x = j * dx;
            qreal y = i * dy;

            // Décalage pour les rangées impaires
            if (j % 2 == 1) {
                y += dy / 2;
            }

            QPolygonF polygon;

            // Créer les sommets de l'hexagone
            for (int k = 0; k < 6; ++k) {
                qreal angle = 2 * M_PI * k / 6.0;
                qreal vertexX = x + adjustedHexRadius * cos(angle);
                qreal vertexY = y + adjustedHexRadius * sin(angle);
                polygon << QPointF(vertexX, vertexY);
            }

            // Ajouter l'hexagone à la collection
            d_meshs.push_back(polygon);
        }
    }
}
