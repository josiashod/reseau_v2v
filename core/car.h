
#ifndef CAR_H
#define CAR_H

#include <functional>
#include "graph.h"
#include <QGraphicsPixmapItem>
// #include <QGraphicsPolygonItem>

class Car
{
 public:
    static constexpr double d_power_threshold = 5.0;
    Car();
     /**
      * @brief Car avec paramètre
      * @param pos: la position de la voiture
      * @param vitesse: la vitesse de la voiture
      * @param frequence: la fréquence de la voiture
      */
    Car(std::vector<osm::Node*>& path, double vitesse = 50.0, double frequence = 60, double intensity = 25);

    size_t id() const;
    /**
     * @brief pos
     * @return
     */
    QPointF pos() const;
    /**
     * @brief acceleration
     */
    double acceleration() const;
    /**
     * @brief frequence
     * @return
     */
    double frequence() const;
    /**
     * @brief intensity
     * @return
     */
    double intensity() const;
    /**
     * @brief accelerate
     * @param nv
     */
    void accelerate(double acc);
    /**
    * @brief pixmap retourne l'image de la voiture (si utilisée)
    */
    QGraphicsPixmapItem* pixmap() const;

    QGraphicsEllipseItem* ellipse() const;

    osm::Node* from() const;
    osm::Node* to() const;

    void randomDestination();

    void nextMove();

    void updateItem();
    void updateOrientation();
    void updatePath(std::vector<osm::Node*>& path);

    bool hasReachedEndOfPath() const;

    double receivedPower(const QPointF& pos) const;

    void update_coverage();

//    void partiallySelected();
//    void selected();
//    void removeSelection();

    /**
     * @brief update mettre à jour la position de la voiture selon un interval
     * @param interval
     */
    void update(double interval);

    QString toString() const;

    bool operator==(int id) const;

 signals:
    void endPathReach(long long currentPos);
 private:
    // vitesse de la voiture
    double d_v;
    // fréquence
    double d_freq;
    // puissance de la voiture
    double d_intensity;
    // vitesse de lecture dans l'animation
    double d_acceleration;

    // position reel de la voiture
    QPointF d_pos;

    // position de depart de la voiture
    osm::Node* d_from;
    // position de position d'arrivée
    osm::Node* d_to;

    std::vector<osm::Node*> d_path;

    size_t i = 1;
    // pixmap pour dessiner la voiture
    QGraphicsPixmapItem* d_pixmap = nullptr;

    double d_elapsed = 0.0; // Temps écoulé dans l'animation

//     // cercle pour représenter la fréquence de la voiture
    QGraphicsEllipseItem* d_ellipse = nullptr;
    static size_t d_compteur_instance;
    size_t d_id;
    QColor d_color;
 };

#endif // CAR_H
