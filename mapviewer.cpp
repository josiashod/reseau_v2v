#include "mapviewer.h"
#include <QGraphicsRectItem>


// CRÉATION DE L'INTERFACE

MapViewer::MapViewer(QWidget *parent)
    : QGraphicsView{parent}
{
    creerInterface();
}

MapViewer::~MapViewer()
{
}

void MapViewer::creerInterface()
{
    setMouseTracking(true);
    setRenderHint(QPainter::Antialiasing);
    d_scene = new QGraphicsScene(this);

    setScene(d_scene);
    d_scene->setBackgroundBrush(QColor(240, 0, 240)); // Gris clair
}

void MapViewer::resizeEvent(QResizeEvent *event)
{
    // Mettre à jour la taille de la scène lors du redimensionnement de la vue
    QGraphicsView::resizeEvent(event);
    setSceneRect(0, 0, event->size().width(), event->size().height());
}

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

QPointF MapViewer::latLonToXY(double lat, double lon)
{
    double width = this->width(), height = this->height();

    double x = (lon - d_minCoord.first) / (d_maxCoord.first - d_minCoord.first) * width;
    double y = height - (lat - d_minCoord.second) / (d_maxCoord.second - d_minCoord.second) * width;

    return {x, y};
}

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
