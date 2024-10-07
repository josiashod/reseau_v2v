#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
//#include <QPainter>
#include <QPoint>
#include <map>
#include <proj.h>

#include "node.h"
#include "noded.h"
#include "way.h"
#include "building.h"
#include "water.h"
#include "park.h"
#include "dbmanager.h"


class MapViewer : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapViewer(QWidget *parent = nullptr);
    ~MapViewer();

    /**
     * @brief findNodeById retourne le noeud correspondant à l'id donné
     * @param id: id du noeud à retourner
     * @return nullptr si aucun noeud trouvé sinon retourn un pointeur sur noeud
     */
    Node* findNodeById(unsigned int id);
    /**
     * @brief findWayById retourne la voie en fonction de l'id
     * @param id
     * @return
     */
    Way* findWayById(unsigned int id);

public slots :
    void map_update();

private:
    // database handler
    DBManager d_db;
    // lon, lat coord
    std::pair<double, double> d_maxCoord, d_minCoord;
    std::map<unsigned int, Node> d_nodes;
    std::map<QString, NodeD> d_descriptifNodes;
    std::map<unsigned int, Water> d_waters;
    std::map<unsigned int, Park> d_parks;

    std::map<unsigned int, Way> d_roads;
    std::map<unsigned int, Building> d_buildings;
//    QVector<QPolygonF> d_meshs;

    double d_scale_factor = 1.15;

    // Permet d'afficher les différentes scenes
    bool d_showPark         = false;
    bool d_showWater        = false;
    bool d_showBuilding     = false;
    bool d_showWay          = false;
    bool d_showDescription  = true;
//    bool d_showMesh  = true;

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
    QGraphicsItemGroup* d_parcLayer;
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

    /**
     * @brief Dessiner la couche de
     */
    void drawRoadLayer();

    /**
     * Dessiner la couche des bâtiments
     */
    void drawBuildingLayer();

    /**
     * Dessiner la couche des bâtiments
     */
    void drawParkLayer();

//    /**
//     * Dessiner la couche des mailles
//     */
//    void drawMeshLayer();

    /**
     * Dessiner la couche d'eau
     */
    void drawWaterLayer();

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
    void initNodes();
//    void initMeshs();
};

#endif // MAPVIEWER_H
