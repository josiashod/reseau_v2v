#include "mapwidget.h"
#include <QGraphicsItemAnimation>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QMenu>
#include "../../core/graph.h"
#include "./../../core/way.h"
#include "./../../core/building.h"
//#include "./../../core/water.h"
#include "./../../core/park.h"
#include "./../../utils/dbmanager.h"


// CRÉATION DE L'INTERFACE

MapWidget::MapWidget(QWidget *parent, osm::Graph* graph)
    : QGraphicsView{parent}, d_graph{graph}
{
    creerInterface();
    initBounds();
}

MapWidget::~MapWidget()
{
//    delete d_parkLayer;
//    delete d_wayLayer;
//    delete d_buildingLayer;
//    delete d_freqCarsLayer;
//    delete d_carsLayer;
    for (auto *item : d_scene->items()) {
        delete item; // Libère chaque élément
    }
    d_scene->clear();
    delete d_scene;
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

    d_freqCarsLayer = new QGraphicsItemGroup();
    d_freqCarsLayer->setVisible(d_showCarFreq);
    d_freqCarsLayer->setAcceptedMouseButtons(Qt::AllButtons);
    d_scene->addItem(d_freqCarsLayer);

    d_carsLayer = new QGraphicsItemGroup();
    d_carsLayer->setVisible(d_showCar);
    d_carsLayer->setAcceptedMouseButtons(Qt::AllButtons);
    d_scene->addItem(d_carsLayer);

    d_meshLayer = new QGraphicsItemGroup();
    d_meshLayer->setVisible(d_showMesh);
    d_meshLayer->setAcceptedMouseButtons(Qt::NoButton);
    d_scene->addItem(d_meshLayer);

//    d_carsLayer->setZValue(1);
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    // Mettre à jour la taille de la scène lors du redimensionnement de la vue
    QGraphicsView::resizeEvent(event);

    if(!d_elementsHasBeenLoaded)
    {
        emit isLoading(d_elementsHasBeenLoaded);

        setRenderHint(QPainter::Antialiasing);
        d_scene->setSceneRect(0, 0, event->size().width() * 2.5, event->size().height() * 2.5);
//        observation_point = QPointF(event->size().width() / 2, event->size().height() / 2);
        fitInView(d_scene->sceneRect(), Qt::KeepAspectRatio);

        auto cleanWatcher = [](QFutureWatcher<void>* watcher) {
            watcher->deleteLater();
        };

        // Configurer la vue (taille et centrage)
        setAlignment(Qt::AlignCenter);

        auto roadWatcher = new QFutureWatcher<void>(this);
        auto parkWatcher = new QFutureWatcher<void>(this);
       auto buildingWatcher = new QFutureWatcher<void>(this);
        auto meshWatcher = new QFutureWatcher<void>(this);

        connect(roadWatcher, &QFutureWatcher<void>::finished, this, [this, cleanWatcher, roadWatcher](){
            isLoadingFinished();
            cleanWatcher(roadWatcher);
        });

        connect(parkWatcher, &QFutureWatcher<void>::finished, this, [ cleanWatcher, parkWatcher](){
            cleanWatcher(parkWatcher);
        });

       connect(buildingWatcher, &QFutureWatcher<void>::finished, this, [this, cleanWatcher, buildingWatcher](){
           cleanWatcher(buildingWatcher);
       });
        connect(meshWatcher, &QFutureWatcher<void>::finished, this, [ cleanWatcher, meshWatcher](){
            cleanWatcher(meshWatcher);
        });

        // Associe le watcher au future
        parkWatcher->setFuture(QtConcurrent::run([this]() {
            initParks();
        }));
        roadWatcher->setFuture(QtConcurrent::run([this]() {
            initRoads();
        }));
       buildingWatcher->setFuture(QtConcurrent::run([this]() {
           initBuildings();
       }));
        meshWatcher->setFuture(QtConcurrent::run([this]() {
            initMeshs();
        }));
    }
}


//void MapWidget::mousePressEvent(QMouseEvent *event)
//{
//    if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
//    {
//        QGraphicsItem *item = itemAt(event->pos());
//        if (d_carsLayer->isAncestorOf(item) || d_freqCarsLayer->isAncestorOf(item))
//        {
//            int id = item->data(0).toString().toInt();
//            auto it = std::find(d_partially_selected_elements.begin(), d_partially_selected_elements.end(), id);

//            // element n'est pas selectionné
//            if (it == d_partially_selected_elements.end())
//            {
//                emit addElementToPartialSelection(id);
//                d_partially_selected_elements.push_back(id);
//            }
//            else
//            {
//                emit removeElementFromPartialSelection(id);
//                d_partially_selected_elements.erase(it);
//            }
//        }
//    }
//    else if(event->modifiers() & Qt::ControlModifier)
//    {
//        if(event->buttons() == Qt::LeftButton)
//        {
//            QGraphicsItem *item = itemAt(event->pos());
//            if (d_carsLayer->isAncestorOf(item) || d_freqCarsLayer->isAncestorOf(item))
//            {
//                int id = item->data(0).toString().toInt();
//                auto it = std::find(d_partially_selected_elements.begin(), d_partially_selected_elements.end(), id);

//                // element n'est pas selectionné
//                if (it == d_partially_selected_elements.end())
//                {
//                    d_partially_selected_elements.clear();
//                    emit deletePartialSelection();

//                    emit addElementToPartialSelection(id);
//                    d_partially_selected_elements.push_back(id);
//                }
//            }
//            else
//            {
//                d_partially_selected_elements.clear();
//                emit deletePartialSelection();
//            }
//        }
//    }
//    else
//    {
//        if(event->buttons() == Qt::LeftButton)
//        {
//            d_partially_selected_elements.clear();
//            emit deletePartialSelection();
//        }
//        else
//        {
//            QMenu menu(this);

//            if(!d_partially_selected_elements.empty())
//            {
//                QString elt = (d_partially_selected_elements.size() == 1) ? "l'élément" : "les éléments";

//                QAction *infoAction = menu.addAction("Ajouter " + elt +" à la selection");
//                QAction *deleteAction = menu.addAction("Supprimer " + elt + " selectionnée" + ((d_partially_selected_elements.size() > 1) ? "s" : ""));

//                QAction *selectedAction = menu.exec(event->globalPos());
//                if (selectedAction == infoAction) {
//                    emit persistPartialSelection();
//                    for(const int& id: d_partially_selected_elements)
//                    {
//                        d_selected_elements.push_back(id);
//                    }
//                    d_partially_selected_elements.clear();
//                } else if (selectedAction == deleteAction) {
//                    emit deleteSelectedElement();
//                    d_partially_selected_elements.clear();
//                }
//            }


//    //        if (item) {
//    //            QAction *infoAction = menu.addAction("Afficher les informations");
//    //            QAction *deleteAction = menu.addAction("Supprimer l'élément");

//    //            QAction *selectedAction = menu.exec(event->globalPos());
//    //            if (selectedAction == infoAction) {
//    //                qDebug() << "Afficher les informations de l'élément à la position:" << item->pos();
//    //            } else if (selectedAction == deleteAction) {
//    //                qDebug() << "Supprimer l'élément";
//    //                delete item;  // Supprime l'élément
//    //            }
//    //        } else {
//    //            QAction *resetAction = menu.addAction("Réinitialiser la vue");
//    //            if (menu.exec(event->globalPos()) == resetAction) {
//    //                qDebug() << "Vue réinitialisée";
//    //                resetTransform();  // Réinitialise la transformation
//    //            }
//    //        }
//        }
//    }


//    QGraphicsView::mousePressEvent(event);
//}

//void MapWidget::keyPressEvent(QKeyEvent *event)
//{
//    if (event->modifiers() == Qt::ControlModifier || (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)))
//        setDragMode(QGraphicsView::NoDrag);  // Permettre la sélection
//    else
//        setDragMode(QGraphicsView::ScrollHandDrag);  // Rester en mode défilement
//    QGraphicsView::keyPressEvent(event);
//}

//void MapWidget::keyReleaseEvent(QKeyEvent *event)
//{
//    setDragMode(QGraphicsView::ScrollHandDrag);  // Rester en mode défilement
//    QGraphicsView::keyPressEvent(event);
//}

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
        if(d_scale_factor > 0.7)
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
    double width = d_scene->sceneRect().width();
    double height = d_scene->sceneRect().height();

//    double x = (lon - d_minCoord.first) / (d_maxCoord.first - d_minCoord.first) * width;
//    double y = height - (lat - d_minCoord.second) / (d_maxCoord.second - d_minCoord.second) * height;

//    double scaleFactor = 2.0; // Agrandir 2 fois

    double x = (lon - d_minCoord.first) / (d_maxCoord.first - d_minCoord.first) * width;
    double y = height - (lat - d_minCoord.second) / (d_maxCoord.second - d_minCoord.second) * height;
    return {x, y};

//    return {x, y * d_perspective_offset};
//    return {x, y};
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
    query.finish();
    // DBManager::instance().close();
}

void MapWidget::initBuildings()
{
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

                    //            coord = lambert93(lon, lat);
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

/*void MapWidget::initWaters()
{
    QVector<Water> waters;
    // auto d_dbmanager = DBManager();
    auto query = DBManager::instance().getWaters();
    bool success = false;

    success = query.exec();

    if(success)
    {
        while(query.next())
        {
            long long id;
            id = query.value(0).toString().toLongLong();

            Water w{id};

            auto q = DBManager::instance().getWayNodes(id);
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
        query.finish();
    }
    // else
    // {
    //     d_logger->addLog("[ERREUR] Un problème est survenu lors de la récupération des waters", LogWidget::DANGER);
    // }
    // d_logger->addLog("[INFO] Emission des waters pour affichage.");
    emit watersDataReady(waters);
// DBManager::instance().close();
}*/

void MapWidget::initRoads()
{
//    QVector<Way> ways;
    // auto d_dbmanager = DBManager();
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
            /*Way w{id};
            QString key = query.value(4).toString();
            QString value = query.value(5).toString();
            w.addTag(key, value);*/


            auto q = DBManager::instance().getWayNodes(id);
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
                        points.push_back(p);
                        //w.addPoint(p);
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
//                    qDebug() << QString("[SUCCESS] Road n°: %1.").arg(id);
                }
//                ways.push_back(w);
                Way w{id, points}
                QMetaObject::invokeMethod(this, [layer = d_wayLayer, w]() {
                   //w.draw(d_wayLayer);
                    layer->addToGroup(w);
                }, Qt::QueuedConnection);
            }
            // if(d_logger)
            // d_logger->addLog(QString("[SUCCESS] Road n°: %1.").arg(id), LogWidget::SUCCESS);
        }
        query.finish();
    }
    // else
    // {
    //     if(d_logger)
    //         d_logger->addLog("[ERREUR] Un problème est survenu lors de la récupération des roads", LogWidget::DANGER);
    // }

    // if(d_logger)
    //     d_logger->addLog("[INFO] Emission des roads pour affichage.");
//    emit roadsDataReady(ways);
    // DBManager::instance().close();
}

void MapWidget::initMeshs()
{
    qreal hexRadius = 100.0;  // Ajuster la taille au besoin
    qreal dx = 1.5 * hexRadius;  // Décalage horizontal (distance entre centres des hexagones)
    qreal dy = sqrt(3) * hexRadius;  // Décalage vertical (distance entre centres des hexagones)


    QRectF scene_rect = d_scene->sceneRect();
    QPointF topLeft = scene_rect.topLeft();    // Coin supérieur gauche
    QPointF bottomRight = scene_rect.bottomRight();  // Coin inférieur droit

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

            // Ajouter l'hexagone à la collection
//            d_meshs.push_back(polygon);

            QMetaObject::invokeMethod(this, [this, hexagon]() {
                QGraphicsPolygonItem *polygon = new QGraphicsPolygonItem();
                polygon->setPolygon(hexagon);

                d_meshLayer->addToGroup(polygon);
            }, Qt::QueuedConnection);
        }
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

void MapWidget::setShowFreq(bool show)
{
    d_showCarFreq = show;
    d_freqCarsLayer->setVisible(d_showCarFreq);
}

void MapWidget::setShowHex(bool show)
{
    d_showMesh = show;
    d_meshLayer->setVisible(d_showMesh);
}
