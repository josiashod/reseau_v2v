#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QPoint>
#include <QMap>


namespace osm
{
    class Graph;
}

class Car;

class MapWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr, osm::Graph* graph = nullptr);
    ~MapWidget();

    void setShowPark(bool);
    void setShowBuilding(bool);
    void setShowRoad(bool);
    void setShowHex(bool);
    void addCar(Car*);
    void addCarEllipse(QGraphicsEllipseItem*);

    static QPointF observation_point;
private slots :
    void isLoadingFinished();

signals :
    void isLoading(bool);
    void isLoaded(bool);
    // emet l'id du car avec la selection partielle
//    void addElementToPartialSelection(int id);
//    void removeElementFromPartialSelection(int id);
//    void deletePartialSelection();
//    void persistPartialSelection();
//    void deleteSelectedElement();

private:
    // lon, lat coord
    std::pair<double, double> d_maxCoord, d_minCoord;

    // stocke les elements de la vue qui ont été partiellement selectionné
   // std::vector<int> d_partially_selected_elements = std::vector<int>(0);
   // std::vector<int> d_selected_elements = std::vector<int>(0);

    double d_scale_factor = 1.15;
    qreal d_perspective_offset = 0.6;

    // Permet d'afficher les différentes scenes
    bool d_showPark         = true;
    bool d_showBuilding     = true;
    bool d_showWay          = true;
    bool d_showDescription  = true;
    bool d_showCar          = true;
    bool d_showMesh         = false;

    // permet de savoir si les elements de la carte on été chargés
    bool d_elementsHasBeenLoaded = false;

    osm::Graph* d_graph;

    // used to prevent view double resized;
    bool d_hasBeenResized;
    /**
     * @brief d_scene scene de la map
     */
    QGraphicsScene*     d_scene;
    /**
    * @brief d_waterLayer couche d'affichage de l'eau
    */
    QGraphicsItemGroup* d_waterLayer;
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
    /**
     * @brief d_carsLayer couche d'affichage des voitures
     */
    QGraphicsItemGroup* d_carsLayer;
    /**
     * @brief d_meshLayer couche d'affichage des mailles
     */
    QGraphicsItemGroup* d_meshLayer;

    void creerInterface();
    void resizeEvent(QResizeEvent *event) override;
    // zoom et deplacement sur la carte
    void wheelEvent(QWheelEvent *event) override;
//    void mousePressEvent(QMouseEvent *event) override;
//    void keyPressEvent(QKeyEvent *event) override;
//    void keyReleaseEvent(QKeyEvent *event) override;
//    void contextMenuEvent(QContextMenuEvent *event) override;

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
    void initBuildings();
    void initParks();
    void initWaters();
    void initRoads();
    void initMeshs();
};

#endif // MAPWIDGET_H
