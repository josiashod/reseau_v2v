#include "mapviewer.h"
#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QtConcurrent>


// CRÉATION DE L'INTERFACE

MapViewer::MapViewer(QWidget *parent)
    : QGraphicsView{parent}, d_descriptifNodes{}
{
    creerInterface();
//    initMeshs();
    initBounds();
}

MapViewer::~MapViewer()
{
    DBManager::destroyInstance();
}

void MapViewer::creerInterface()
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

    connect(this, &MapViewer::buildingsDataReady, this, &MapViewer::drawBuildingLayer);
    connect(this, &MapViewer::parksDataReady, this, &MapViewer::drawParkLayer);
    connect(this, &MapViewer::watersDataReady, this, &MapViewer::drawWaterLayer);
    connect(this, &MapViewer::roadsDataReady, this, &MapViewer::drawRoadLayer);
}

void MapViewer::drawDescriptionLayer()
{
    for (auto it = d_descriptifNodes.begin(); it != d_descriptifNodes.end(); ++it)
    {
        // Créer un nouvel élément texte
        QGraphicsTextItem *textItem = new QGraphicsTextItem(it->second.name());
        textItem->setPos(it->second);
        QFont font = textItem->font();
        font.setPointSize(2 / d_scale_factor);  // Ajuster la taille de la police
        textItem->setFont(font);

        d_descriptionLayer->addToGroup(textItem);
    }
}

void MapViewer::drawBuildingLayer(const QVector<Building>& buildings)
{
//    for (auto it = d_buildings.begin(); it != d_buildings.end(); ++it)
//    {
//        it->second.draw(d_buildingLayer, d_scale_factor);
//    }
    for (const Building& b : buildings)
    {
        b.draw(d_buildingLayer, d_scale_factor); // Dessine dans le thread principal
    }
}

void MapViewer::drawWaterLayer(const QVector<Water>& waters)
{
    for (const Water& w : waters)
    {
        w.draw(d_waterLayer, d_scale_factor); // Dessine dans le thread principal
    }
}
void MapViewer::drawParkLayer(const QVector<Park>& parks)
{
    for (const Park& p : parks)
    {
        p.draw(d_parkLayer, d_scale_factor); // Dessine dans le thread principal
    }
}
void MapViewer::drawRoadLayer(const QVector<Way>& ways)
{
    for (const Way& w : ways)
    {
        w.draw(d_wayLayer, d_scale_factor); // Dessine dans le thread principal
    }
}

//void MapViewer::drawMeshLayer()
//{
//    for (QPolygonF& hexagon : d_meshs)
//    {
//        // Créer un nouvel élément texte
//        QGraphicsPolygonItem *polygon = new QGraphicsPolygonItem();
//        polygon->setPolygon(hexagon);

//        d_meshLayer->addToGroup(polygon);
//    }
//}

void MapViewer::resizeEvent(QResizeEvent *event)
{
    // Mettre à jour la taille de la scène lors du redimensionnement de la vue
    QGraphicsView::resizeEvent(event);

    setRenderHint(QPainter::Antialiasing);
//    d_scene->setSceneRect(0, 0, event->size().width() * 2, event->size().height() * 2);
    fitInView(d_scene->sceneRect(), Qt::KeepAspectRatio);

    initNodeDs();
    // Lancer la fonction longue en asynchrone
//    QtConcurrent::run(this, &MapViewer::initBuildings);
    (void)QtConcurrent::run([this]() {
        this->initWaters();
        this->initParks();
        this->initRoads();
        this->initBuildings();
        DBManager::destroyInstance();
    });
    drawDescriptionLayer();
//    drawBuildingLayer();
//    drawMeshLayer();

    // Configurer la vue (taille et centrage)
    setAlignment(Qt::AlignCenter);
}

void MapViewer::wheelEvent(QWheelEvent *event)
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

//void MapViewer::mousePressEvent(QMouseEvent *event)
//{
//    // Vérifiez si le bouton gauche de la souris est pressé
//    if (event->button() == Qt::LeftButton) {
//        setDragMode(QGraphicsView::ScrollHandDrag);
//        QGraphicsView::mousePressEvent(event);
//    } else {
//        QGraphicsView::mousePressEvent(event);
//    }
//}

//void MapViewer::mouseReleaseEvent(QMouseEvent *event)
//{
//    setDragMode(QGraphicsView::NoDrag);
//    QGraphicsView::mouseReleaseEvent(event);
//}

//void MapViewer::mouseMoveEvent(QMouseEvent *event)
//{
//    setDragMode(QGraphicsView::NoDrag);
//    QGraphicsView::mouseReleaseEvent(event);
//}


std::pair<double, double> MapViewer::lambert93(double longitude, double latitude)
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
}

QPointF MapViewer::pairLatLonToXY(std::pair<double, double>& coord)
{
    return latLonToXY(coord.first, coord.second);
}

QPointF MapViewer::latLonToXY(double lon, double lat)
{
    double width = this->width(), height = this->height();

    double x = (lon - d_minCoord.first) / (d_maxCoord.first - d_minCoord.first) * width;
    double y = height - (lat - d_minCoord.second) / (d_maxCoord.second - d_minCoord.second) * width;

    return {x, y};
}

void MapViewer::initBounds()
{
    auto db = DBManager::getInstance();
    QSqlQuery query = db->getBounds(db->getDatabase());
    double minLat = 0.0, maxLat = 0.0, minLon = 0.0, maxLon = 0.0;
    bool success = false;

    success = query.exec();

    if (success && query.next()) {
        // je récupère les bornes de la table nodes
        minLat = query.value("min_lat").toDouble();
        maxLat = query.value("max_lat").toDouble();
        minLon = query.value("min_lon").toDouble();
        maxLon = query.value("max_lon").toDouble();

        // Applique la transformation Lambert93 aux bornes
        d_maxCoord = lambert93(maxLon, maxLat);
        d_minCoord = lambert93(minLon, minLat);

        // Vérification des bornes projetées
        qDebug() << "Max Coord (Lambert93):" << d_maxCoord;
        qDebug() << "Min Coord (Lambert93):" << d_minCoord;
    } else {
        qDebug() << "Erreur lors de l'exécution de la requête de bornes.";
    }

    query.clear();
}

void MapViewer::initNodeDs()
{
    auto db = DBManager::getInstance();
    QSqlQuery query = db->getNodeDs(db->getDatabase());
    bool success = false;

    success = query.exec();
    //    success = query.exec();
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

            coord = lambert93(lon, lat);
            name = query.value(6).toString();
            NodeD n{id, pairLatLonToXY(coord), name};
            d_descriptifNodes.emplace(name, n);
        }
    }
    query.clear();
}

//Node* MapViewer::findNodeById(long long id)
//{
//    if(d_nodes.find(id) != d_nodes.end())
//        return &d_nodes.at(id);
//    else
//        return nullptr;
//}

//void MapViewer::initNodes()
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
//            Node n{id, pairLatLonToXY(coord)};
//            d_nodes.emplace(id, n);

//            qDebug() << "Nodes: " << id;
//        }
//    }
//}

void MapViewer::initBuildings()
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

                    coord = lambert93(lon, lat);
                    Node n{id, pairLatLonToXY(coord)};
                    b.addNode(n);
                }
            }
            buildings.push_back(b);
            qDebug() << "building: " << id;
        }
        query.clear();
        emit buildingsDataReady(buildings);
        qDebug() << "emit buildings";
    }
    else
        qDebug() << "erreur";
}

void MapViewer::initParks()
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

                    coord = lambert93(lon, lat);
                    Node n{id, pairLatLonToXY(coord)};
                    p.addNode(n);
                }
            }
            parks.push_back(p);
            qDebug() << "park: " << id;
        }
        query.clear();
        emit parksDataReady(parks);
    }
    else
        qDebug() << "erreur";
}

void MapViewer::initWaters()
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

                    coord = lambert93(lon, lat);
                    Node n{id, pairLatLonToXY(coord)};
                    w.addNode(n);
                }
            }
            waters.push_back(w);
            qDebug() << "water: " << id;
        }
        query.clear();
        emit watersDataReady(waters);
        qDebug() << "emits waters";
    }
    else
        qDebug() << "erreur";
}

void MapViewer::initRoads()
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
                    std::pair<double, double> coord;
                    long long id;
                    double lat = 0.0, lon = 0.0;

                    id = q.value(0).toString().toLongLong();
                    lat = q.value(1).toString().toDouble();
                    lon = q.value(2).toString().toDouble();

                    coord = lambert93(lon, lat);
                    Node n{id, pairLatLonToXY(coord)};
                    w.addNode(n);
                }
            }
            ways.push_back(w);
            qDebug() << "ways: " << id;
        }
        query.clear();
        emit roadsDataReady(ways);
        qDebug() << "emit ways";
    }
    else
        qDebug() << "erreur";
}

//void MapViewer::initMeshs()
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


void MapViewer::map_update()
{
    //update();
}
