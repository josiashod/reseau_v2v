#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QWidget>
#include <QGraphicsView>
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
#include "parc.h"


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
    Way* findWayById(unsigned int id);

public slots :
    void map_update();

private:
    // lon, lat coord
    std::pair<double, double> d_maxCoord, d_minCoord;
    std::map<unsigned int, Node> d_nodes;
    std::map<unsigned int, NodeD> d_descriptiveNodes;
    std::map<unsigned int, Water> d_waters;
    std::map<unsigned int, Parc> d_parcs;

    std::map<unsigned int, Way> d_roads;
    std::map<unsigned int, Building> d_buildings;

    // Permet d'afficher les différentes scenes
    bool d_showParc         = false;
    bool d_showWater        = false;
    bool d_showBuilding     = false;
    bool d_showWay          = false;
    bool d_showDescription  = false;

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
    /**
     * @brief d_meshLayer couche d'affichage des mailles
     */
    QGraphicsItemGroup* d_meshLayer;

    void creerInterface();
    void resizeEvent(QResizeEvent *event) override;

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
    void drawParcLayer();

    /**
     * Dessiner la couche des mailles
     */
    void drawMeshLayer();

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
     * @brief latLonToXY Convertit les coordonnées géographiques (latitude et longitude)
     *        en coordonnées X, Y pour un affichage sur une fenêtre.
     * @param lat: La latitude du point à convertir.
     * @param lon: La longitude du point à convertir.
     * @return un QPointF
     */
    QPointF latLonToXY(double lat, double lon);
};

#endif // MAPVIEWER_H
