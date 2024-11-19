
#ifndef CAR_H
#define CAR_H

 #include "graph.h"
 #include <QGraphicsPixmapItem>
// #include <QGraphicsPolygonItem>

 class Car
 {
 public:
    Car();
     /**
      * @brief Car avec paramètre
      * @param pos: la position de la voiture
      * @param vitesse: la vitesse de la voiture
      * @param frequence: la fréquence de la voiture
      */
    Car(osm::Node* from, double vitesse = 30.9, double frequence = 10.5, double intensity = 10);

//     /**
//      * @brief Car constructeur de recopie
//      */
//    Car(const Car&);

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

    void randomDestination();

    void nextMove();

//    void centerPixmap();

    /**
     * @brief update mettre à jour la position de la voiture selon un interval
     * @param interval
     */
    void update(double interval);

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

    // pixmap pour dessiner la voiture
    QGraphicsPixmapItem* d_pixmap = nullptr;

    double d_elapsed = 0.0; // Temps écoulé dans l'animation

//     // cercle pour représenter la fréquence de la voiture
    QGraphicsEllipseItem* d_ellipse = nullptr;

    double d_sens = 1;
 };

#endif // CAR_H
