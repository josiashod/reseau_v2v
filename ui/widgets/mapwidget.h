#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
//#include <QPainter>
#include <QPoint>
#include <map>
#include <proj.h>

#include "./../../core/node.h"
#include "./../../core/noded.h"
#include "./../../core/way.h"
#include "./../../core/building.h"
#include "./../../core/water.h"
#include "./../../core/park.h"
#include "./../../utils/dbmanager.h"


class MapWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

//    /**
//     * @brief findNodeById retourne le noeud correspondant à l'id donné
//     * @param id: id du noeud à retourner
//     * @return nullptr si aucun noeud trouvé sinon retourn un pointeur sur noeud
//     */
//    Node* findNodeById(long long id);
//    /**
//     * @brief findWayById retourne la voie en fonction de l'id
//     * @param id
//     * @return
//     */
//    Way* findWayById(long long id);

    void setShowPark(bool);
    void setShowBuilding(bool);
    void setShowRoad(bool);
    void setShowWater(bool);
    void setShowDescription(bool);

private slots :
//    void map_update();
    /**
     * Dessiner la couche des bâtiments
     */
    void drawBuildingLayer(const QVector<Building>& buildings);
    void drawWaterLayer(const QVector<Water>& waters);
    void drawParkLayer(const QVector<Park>& parks);
    /**
     * @brief Dessiner la couche de
     */
    void drawRoadLayer(const QVector<Way>& roads);

signals :
    void buildingsDataReady(const QVector<Building>& buildings);
    void parksDataReady(const QVector<Park>& parks);
    void watersDataReady(const QVector<Water>& waters);
    void roadsDataReady(const QVector<Way>& roads);

private:
    // database handler
//    DBManager d_db;
    // lon, lat coord
    std::pair<double, double> d_maxCoord, d_minCoord;
//    std::map<long long, Node> d_nodes;
    std::map<QString, NodeD> d_descriptifNodes;
//    std::map<long long, Water> d_waters;
//    std::map<long long, Park> d_parks;

    const QVector<Way> d_roads;
//    std::map<long long, Building> d_buildings;
//    QVector<QPolygonF> d_meshs;

    double d_scale_factor = 1.15;

    // Permet d'afficher les différentes scenes
    bool d_showPark         = true;
    bool d_showWater        = true;
    bool d_showBuilding     = true;
    bool d_showWay          = true;
    bool d_showDescription  = true;
//    bool d_showMesh  = true;

    // permet de savoir si les elements de la carte on été chargés
    bool d_elementsHasBeenLoaded = false;

    /**
     * @brief d_view la vue graphique pour la map
     */
//    QGraphicsView* d_view;
    /**
     * @brief d_scene scene de la map
     */
    QGraphicsScene*     d_scene;
    /**
     * @brief d_waterLayer couche d'affichage de l'eau
     */
    QGraphicsItemGroup* d_waterLayer;
    /**
     * @brief d_descriptionLayer couche d'affichage des descriptions
     */
    QGraphicsItemGroup* d_descriptionLayer;
    /**
     * @brief d_parcLayer couche d'affichage des espaces verts
     */
    QGraphicsItemGroup* d_parkLayer;
    /**
     * @brief d_buildingLayer couche d'affichage des batiments
     */
    QGraphicsItemGroup* d_buildingLayer;
    /**
     * @brief d_wayLayer couche d'affichage des routes
     */
    QGraphicsItemGroup* d_wayLayer;
//    /**
//     * @brief d_meshLayer couche d'affichage des mailles
//     */
//    QGraphicsItemGroup* d_meshLayer;

    void creerInterface();
    void resizeEvent(QResizeEvent *event) override;

    // zoom et deplacement sur la carte
    void wheelEvent(QWheelEvent *event) override;

    /**
     * @brief Dessiner la couche de description
     */
    void drawDescriptionLayer();

//    /**
//     * Dessiner la couche des bâtiments
//     */
    /*void drawParkLayer()*/;

//    /**
//     * Dessiner la couche des mailles
//     */
//    void drawMeshLayer();

//    /**
//     * Dessiner la couche d'eau
//     */
//    void drawWaterLayer();

//    void paintEvent(QPaintEvent *) override;
//    void dessinePolygone(QPainter &painter);
//    void dessinerHexagone(QPainter &painter, double cx, double cy, double r);

    /**
     * @brief lambert93 Convertis les coordonnees WSG84 en lambert93
     * @return le couple {double lon, double lat}
     */
    std::pair<double, double> lambert93(double, double);
    /**
     * @brief pairLatLonToXY Convertit les coordonnées géographiques (latitude et longitude)
     *        en coordonnées X, Y pour un affichage sur une fenêtre.
     * @param pair {lon lat}
     * @return un QPointF
     */
    QPointF pairLatLonToXY(std::pair<double, double>&);
    /**
     * @brief latLonToXY Convertit les coordonnées géographiques (latitude et longitude)
     *        en coordonnées X, Y pour un affichage sur une fenêtre.
     * @param lat: La latitude du point à convertir.
     * @param lon: La longitude du point à convertir.
     * @return un QPointF
     */
    QPointF latLonToXY(double lon, double lat);

    void initBounds();
    void initNodeDs();
//    void initNodes();
    void initBuildings();
    void initParks();
    void initWaters();
    void initRoads();
//    void initMeshs();
};

#endif // MAPWIDGET_H
