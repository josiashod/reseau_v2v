#include "mapwidget.h"
#include <QGraphicsItemAnimation>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include "logwidget.h"


// CRÉATION DE L'INTERFACE

MapWidget::MapWidget(QWidget *parent)
    : QGraphicsView{parent}
{
    creerInterface();
//    initMeshs();
    initBounds();
}

MapWidget::~MapWidget()
{
    DBManager::destroyInstance();
}

void MapWidget::creerInterface()
{
//    QVBoxLayout *layout = new QVBoxLayout(this);
//    layout->setContentsMargins(0, 0, 0, 0);
//    setLayout(layout);

//    d_view = new QGraphicsView{this};

    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag);

    d_scene = new QGraphicsScene(this);
    d_scene->setBackgroundBrush(QColor("#F2EFE9")); // Gris clair
    setScene(d_scene);
    show();

    d_wayLayer = new QGraphicsItemGroup();
    d_wayLayer->setVisible(d_showWay);
    d_scene->addItem(d_wayLayer);

    d_waterLayer = new QGraphicsItemGroup();
    d_waterLayer->setVisible(d_showWater);
    d_scene->addItem(d_waterLayer);

    d_parkLayer = new QGraphicsItemGroup();
    d_parkLayer->setVisible(d_showPark);
    d_scene->addItem(d_parkLayer);

    d_buildingLayer = new QGraphicsItemGroup();
    d_buildingLayer->setVisible(d_showBuilding);
    d_scene->addItem(d_buildingLayer);

    d_descriptionLayer = new QGraphicsItemGroup();
    d_descriptionLayer->setVisible(d_showDescription);
    d_scene->addItem(d_descriptionLayer);


//    d_meshLayer = new QGraphicsItemGroup();
//    d_meshLayer->setVisible(d_showMesh);
//    d_scene->addItem(d_meshLayer);

//    layout->addWidget(d_view);

    connect(this, &MapWidget::desciptionNodesDataReady, this, &MapWidget::drawDescriptionLayer);
    connect(this, &MapWidget::buildingsDataReady, this, &MapWidget::drawBuildingLayer);
    connect(this, &MapWidget::parksDataReady, this, &MapWidget::drawParkLayer);
    connect(this, &MapWidget::watersDataReady, this, &MapWidget::drawWaterLayer);
    connect(this, &MapWidget::roadsDataReady, this, &MapWidget::drawRoadLayer);
}

void MapWidget::drawDescriptionLayer(const std::map<QString, NodeD>& nodes)
{
    for (auto it = nodes.begin(); it != nodes.end(); ++it)
    {
        // Créer un nouvel élément texte
        QGraphicsTextItem *textItem = new QGraphicsTextItem(it->second.name());
        textItem->setPos(it->second);
        QFont font = textItem->font();
        font.setPointSize(8 / d_scale_factor);  // Ajuster la taille de la police
        textItem->setFont(font);

        d_descriptionLayer->addToGroup(textItem);
    }
}

void MapWidget::drawBuildingLayer(const QVector<Building>& buildings)
{
    QString log;
    for (const Building& b : buildings)
    {
        b.draw(d_buildingLayer, d_scale_factor); // Dessine dans le thread principal
        log = QString("[INFO] Drawing building n°: %1.").arg(b.id());
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
            qDebug() << log;
    }
}

void MapWidget::drawWaterLayer(const QVector<Water>& waters)
{
    QString log;
    for (const Water& w : waters)
    {
        w.draw(d_waterLayer, d_scale_factor); // Dessine dans le thread principal
        log = QString("[INFO] Drawing water n°: %1.").arg(w.id());
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
            qDebug() << log;
    }
}

void MapWidget::drawParkLayer(const QVector<Park>& parks)
{
    QString log;
    for (const Park& p : parks)
    {
        p.draw(d_parkLayer, d_scale_factor); // Dessine dans le thread principal
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
        w.draw(d_wayLayer, d_scale_factor); // Dessine dans le thread principal
        log = QString("[INFO] Drawing road n°: %1.").arg(w.id());
        // if(d_logger)
        //     d_logger->addLog(log);
        // else
            qDebug() << log;
    }
}

//void MapWidget::drawMeshLayer()
//{
//    for (QPolygonF& hexagon : d_meshs)
//    {
//        // Créer un nouvel élément texte
//        QGraphicsPolygonItem *polygon = new QGraphicsPolygonItem();
//        polygon->setPolygon(hexagon);

//        d_meshLayer->addToGroup(polygon);
//    }
//}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    // Mettre à jour la taille de la scène lors du redimensionnement de la vue
    QGraphicsView::resizeEvent(event);

    if(!d_elementsHasBeenLoaded)
    {
        emit isLoading(d_elementsHasBeenLoaded);

        setRenderHint(QPainter::Antialiasing);

    //    d_scene->setSceneRect(0, 0, event->size().width() * 2, event->size().height() * 2);
        fitInView(d_scene->sceneRect(), Qt::KeepAspectRatio);
        // Lancer la fonction longue en asynchrone
        QFuture<void> future = QtConcurrent::run([this]() {
            DBManager::getInstance();
            this->initNodeDs();
            this->initWaters();
            this->initParks();
            this->initRoads();
            this->initBuildings();
            DBManager::destroyInstance();
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

//    // Appliquer le nouveau facteur d'échelle aux éléments si nécessaire
//    for (QGraphicsItem* item : d_descriptionLayer->childItems() ) {
//        // Appliquer l'échelle uniquement aux éléments spécifiques, comme les textes ou les ellipses
//        item->setScale(d_scale_factor);
//    }
}

//void MapWidget::mousePressEvent(QMouseEvent *event)
//{
//    // Vérifiez si le bouton gauche de la souris est pressé
//    if (event->button() == Qt::LeftButton) {
//        setDragMode(QGraphicsView::ScrollHandDrag);
//        QGraphicsView::mousePressEvent(event);
//    } else {
//        QGraphicsView::mousePressEvent(event);
//    }
//}

//void MapWidget::mouseReleaseEvent(QMouseEvent *event)
//{
//    setDragMode(QGraphicsView::NoDrag);
//    QGraphicsView::mouseReleaseEvent(event);
//}

//void MapWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    setDragMode(QGraphicsView::NoDrag);
//    QGraphicsView::mouseReleaseEvent(event);
//}


//std::pair<double, double> MapWidget::lambert93(double longitude, double latitude)
//{
//    // Créer un contexte PROJ
//    PJ_CONTEXT *ctx = proj_context_create();
//    if (ctx == nullptr) {
//        throw std::runtime_error("Failed to create PROJ context.");
//    }

//    // Définir les systèmes de coordonnées avec des chaînes PROJ
//    const char* wgs84 = "+proj=longlat +datum=WGS84 +no_defs";
//    const char* lambert93 = "+proj=lcc +lat_1=49 +lat_2=44 +lat_0=46.5 +lon_0=3 +x_0=700000 +y_0=6600000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs";

//    // Créer la transformation CRS
//    PJ *P = proj_create_crs_to_crs(ctx, wgs84, lambert93, nullptr);
//    if (P == nullptr) {
//        proj_context_destroy(ctx);
//        throw std::runtime_error("Failed to create CRS transformation.");
//    }

//    // Convertir les coordonnées
//    PJ_COORD coord = proj_coord(longitude, latitude, 0, 0);
//    PJ_COORD result = proj_trans(P, PJ_FWD, coord);

//    // Vérifier si la transformation a échoué
//    if (result.xyzt.x == HUGE_VAL || result.xyzt.y == HUGE_VAL) {
//        proj_destroy(P);
//        proj_context_destroy(ctx);
//        throw std::runtime_error("Transformation failed.");
//    }

//    // Libérer les ressources
//    proj_destroy(P);
//    proj_context_destroy(ctx);

//    return {result.xyzt.x, result.xyzt.y}; // Retourner les coordonnées Lambert93
//}

QPointF MapWidget::pairLatLonToXY(std::pair<double, double>& coord)
{
    return latLonToXY(coord.first, coord.second);
}

QPointF MapWidget::latLonToXY(double lon, double lat)
{
    double width = this->width(), height = this->height();

    double x = (lon - d_minCoord.first) / (d_maxCoord.first - d_minCoord.first) * width;
    double y = height - (lat - d_minCoord.second) / (d_maxCoord.second - d_minCoord.second) * width;

    return {x, y};
}

void MapWidget::initBounds()
{
    auto db = DBManager::getInstance();
    QSqlQuery query = db->getBounds(db->getDatabase());
    double minLat = 0.0, maxLat = 0.0, minLon = 0.0, maxLon = 0.0;
    bool success = false;
    QString log;

    success = query.exec();

    if (success && query.next()) {
        // je récupère les bornes de la table nodes
        minLat = query.value("min_lat").toDouble();
        maxLat = query.value("max_lat").toDouble();
        minLon = query.value("min_lon").toDouble();
        maxLon = query.value("max_lon").toDouble();

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

void MapWidget::initNodeDs()
{
    auto db = DBManager::getInstance();
    QSqlQuery query = db->getNodeDs(db->getDatabase());
    bool success = false;
    QString log;
    std::map<QString, NodeD> nodes{};


    success = query.exec();
    if(success)
    {
        while(query.next())
        {
            std::pair<double, double> coord;
            long long id;
            double lat = 0.0, lon = 0.0;
            QString name = "";

            id = query.value(0).toString().toLongLong();
            lat = query.value(1).toString().toDouble();
            lon = query.value(2).toString().toDouble();

            //            coord = lambert93(lon, lat);
            coord = std::make_pair(lon, lat);
            name = query.value(6).toString();
            NodeD n{id, pairLatLonToXY(coord), name};
            nodes.emplace(name, n);
            log = QString("[SUCCESS] Noeuds de description n°: %1 recupérer avec succès.").arg(id);
//            if(d_logger)
//                d_logger->addLog(log, LogWidget::SUCCESS);
//            else
                qDebug() << log;
        }
    }
    query.clear();
    log = "[INFO] Emission des noeuds pour affichage.";
    // if(d_logger)
    //     d_logger->addLog(log);
    // else
        qDebug() << log;
    emit desciptionNodesDataReady(nodes);
}

//Node* MapWidget::findNodeById(long long id)
//{
//    if(d_nodes.find(id) != d_nodes.end())
//        return &d_nodes.at(id);
//    else
//        return nullptr;
//}

//void MapWidget::initNodes()
//{
//    QSqlQuery query;
//    bool success = false;

//    success = query.exec(d_db.getNodes());
//    //    success = query.exec();
//    if(success)
//    {
//        while(query.next())
//        {
//            std::pair<double, double> coord;
//            long long id;
//            double lat = 0.0, lon = 0.0;
//            QString name = "";

//            id = query.value(0).toString().toLongLong();
//            lat = query.value(1).toString().toDouble();
//            lon = query.value(2).toString().toDouble();

//            coord = lambert93(lon, lat);
//            coord = std::make_pair(lon, lat);
//            Node n{id, pairLatLonToXY(coord)};
//            d_nodes.emplace(id, n);

//            qDebug() << "Nodes: " << id;
//        }
//    }
//}

void MapWidget::initBuildings()
{
    QVector<Building> buildings;
    auto db = DBManager::getInstance();
    auto query = db->getBuildings(db->getDatabase());
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Building b{id};

            auto q = db->getWayNodes(db->getDatabase(), id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
                    long long id;
                    double lat = 0.0, lon = 0.0;

                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    //            coord = lambert93(lon, lat);
                    coord = std::make_pair(lon, lat);
                    Node n{id, pairLatLonToXY(coord)};
                    b.addNode(n);
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
    auto db = DBManager::getInstance();
    auto query = db->getParks(db->getDatabase());
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Park p{id};

            auto q = db->getWayNodes(db->getDatabase(), id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
                    long long id;
                    double lat = 0.0, lon = 0.0;

                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    //            coord = lambert93(lon, lat);
                    coord = std::make_pair(lon, lat);
                    Node n{id, pairLatLonToXY(coord)};
                    p.addNode(n);
                }
            }
            parks.push_back(p);
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

void MapWidget::initWaters()
{
    QVector<Water> waters;
    auto db = DBManager::getInstance();
    auto query = db->getWaters(db->getDatabase());
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Water w{id};

            auto q = db->getWayNodes(db->getDatabase(), id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    std::pair<double, double> coord;
                    long long id;
                    double lat = 0.0, lon = 0.0;

                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    //            coord = lambert93(lon, lat);
                    coord = std::make_pair(lon, lat);
                    Node n{id, pairLatLonToXY(coord)};
                    w.addNode(n);
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
}

void MapWidget::initRoads()
{
    QVector<Way> ways;
    auto db = DBManager::getInstance();
    auto query = db->getRoads(db->getDatabase());
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Way w{id};

            w.addTag(query.value(3).toString(), query.value(4).toString());

            auto q = db->getWayNodes(db->getDatabase(), id);
            success = q.exec();
            if(success)
            {
                while(q.next())
                {
                    long long id;
                    double lat = 0.0, lon = 0.0;

                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    //            coord = lambert93(lon, lat);
                    std::pair<double, double> coord{lon, lat};
                    Node n{id, pairLatLonToXY(coord)};
                    w.addNode(n);
                }
            }
            ways.push_back(w);
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

void MapWidget::setShowWater(bool show)
{
    d_showWater = show;
    d_waterLayer->setVisible(show);
}

void MapWidget::setShowDescription(bool show)
{
    d_showDescription = show;
    d_descriptionLayer->setVisible(d_showDescription);
}

//void MapWidget::initMeshs()
//{
//    qreal hexRadius = 8.0;  // Adjust size as needed
//    int rows = height();
//    int cols = width();

//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            qreal x = j * 1.5 * hexRadius;
//            qreal y = i * sqrt(3) * hexRadius;

//            if (j % 2 == 1) {
//                y += sqrt(3) / 2 * hexRadius;
//            }
//            QPolygonF polygon;

//            // Initialize the polygon in the constructor
//            for (int i = 0; i < 6; ++i) {
//                qreal angle = 2 * M_PI * i / 6.0;
//                x = x + hexRadius * cos(angle);
//                y = y + hexRadius * sin(angle);
//                polygon << QPointF(x, y);
//            }
//            d_meshs.push_back(polygon);
//        }
//    }
//}


//void MapWidget::map_update()
//{
//    //update();
//}
