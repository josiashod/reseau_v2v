#include "mapviewer.h"
#include <QGraphicsRectItem>
#include <QVBoxLayout>


// CRÉATION DE L'INTERFACE

MapViewer::MapViewer(QWidget *parent)
    : QGraphicsView{parent}, d_db{}, d_nodes{}, d_descriptifNodes{}, d_waters{},
      d_parks{}, d_roads{}, d_buildings{}
{
    creerInterface();
//    initMeshs();
    initBounds();
}

MapViewer::~MapViewer()
{
}

void MapViewer::creerInterface()
{
//    QVBoxLayout *layout = new QVBoxLayout(this);
//    layout->setContentsMargins(0, 0, 0, 0);
//    setLayout(layout);

//    d_view = new QGraphicsView{this};
    setMouseTracking(true);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag);

    d_scene = new QGraphicsScene(this);
    d_scene->setBackgroundBrush(QColor(240, 240, 240)); // Gris clair
    setScene(d_scene);
    show();

    d_descriptionLayer = new QGraphicsItemGroup();
    d_descriptionLayer->setVisible(d_showDescription);
    d_scene->addItem(d_descriptionLayer);

//    d_meshLayer = new QGraphicsItemGroup();
//    d_meshLayer->setVisible(d_showMesh);
//    d_scene->addItem(d_meshLayer);

//    layout->addWidget(d_view);
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
//    d_scene->setSceneRect(0, 0, event->size().width() * 2, event->size().height() * 2);
    fitInView(d_scene->sceneRect(), Qt::KeepAspectRatio);

    initNodeDs();
    initNodeDs();
    drawDescriptionLayer();
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
    QSqlQuery query;
    double minLat = 0.0, maxLat = 0.0, minLon = 0.0, maxLon = 0.0;
    bool success = false;

    success = query.exec(d_db.getBounds());

    if(success)
    {
        if(query.next())
        {
            minLat = query.value(0).toString().toFloat();
            maxLat = query.value(1).toString().toFloat();
            minLon = query.value(2).toString().toFloat();
            maxLon = query.value(3).toString().toFloat();
        }
    }

    d_maxCoord = lambert93(maxLon, maxLat);
    d_minCoord = lambert93(minLon, minLat);
}

void MapViewer::initNodeDs()
{
    QSqlQuery query;
    bool success = false;

    success = query.exec(d_db.getNodeDs());
    //    success = query.exec();
    if(success)
    {
        while(query.next())
        {
            std::pair<double, double> coord;
            unsigned int id;
            double lat = 0.0, lon = 0.0;
            QString name = "";

            id = query.value(0).toString().toUInt();
            lat = query.value(1).toString().toDouble();
            lon = query.value(2).toString().toDouble();

            coord = lambert93(lon, lat);
            name = query.value(6).toString();
            NodeD n{id, pairLatLonToXY(coord), name};
            d_descriptifNodes.emplace(name, n);
        }
    }
}

void MapViewer::initNodes()
{
    QSqlQuery query;
    bool success = false;

    success = query.exec(d_db.getNodes());
    //    success = query.exec();
    if(success)
    {
        while(query.next())
        {
            std::pair<double, double> coord;
            unsigned int id;
            double lat = 0.0, lon = 0.0;
            QString name = "";

            id = query.value(0).toString().toUInt();
            lat = query.value(1).toString().toDouble();
            lon = query.value(2).toString().toDouble();

            coord = lambert93(lon, lat);
            Node n{id, pairLatLonToXY(coord)};
            d_nodes.emplace(id, n);
        }
    }
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


//void MapViewer::paintEvent(QPaintEvent *)
//{
//    QPainter painter{this};
//    dessinePolygone(painter);

////    DBManager db{};
////    QSqlQuery query;
////    bool success = query.exec("SELECT * FROM nodes");
////    if(success)
////    {
////        while(query.next())
////        {
////            long long id = query.value(0).toString().toLongLong();
////            double lat = query.value(1).toString().toFloat();
////            double lon = query.value(2).toString().toFloat();
////            painter.drawEllipse(latLonToXY(lat, lon), 1, 1);
////        }
////    }
//}

//void MapViewer::dessinePolygone(QPainter &painter)
//{
//    static const QPointF points[4] = {
//        QPointF(10.0, 80.0),
//        QPointF(20.0, 10.0),
//        QPointF(80.0, 30.0),
//        QPointF(90.0, 70.0)
//    };

//    painter.drawPolygon(points, 4);
//}

//void MapViewer::dessinerHexagone(QPainter &painter, double cx, double cy, double r)
//{
//    painter.setPen(Qt::black);
//    QPolygonF hexagon;

//    for (int i = 0; i < 6; ++i) {
//        double angle_rad = 2 * M_PI * i / 6.0;
//        double x = cx + r * std::cos(angle_rad);
//        double y = cy + r * std::sin(angle_rad);
//        hexagon << QPointF(x, y);
//    }

//    painter.drawPolygon(hexagon);
//}

void MapViewer::map_update()
{
    //update();
}
