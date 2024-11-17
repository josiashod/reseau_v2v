#ifndef CAR_H
#define CAR_H

// #include "node.h"
 #include <QGraphicsPixmapItem>
// #include <QGraphicsPolygonItem>

 class Car
 {
 public:
     // constructeur par défaut
    Car();
     /**
      * @brief Car avec paramètre
      * @param pos: la position de la voiture
      * @param vitesse: la vitesse de la voiture
      * @param frequence: la fréquence de la voiture
      */
    Car(const QPointF& pos, double vitesse = 100.0, double frequence = 1.5);

     /**
      * @brief Car constructeur de recopie
      */
    Car(const Car&);

    QPointF pos() const;

//     /**
//      * @brief moveTo déplace la voiture à la nouvelle position donnée
//      */
//     void moveTo();

    /**
    * @brief pixmap retourne l'image de la voiture (si utilisée)
    */
    QGraphicsPixmapItem* pixmap() const;

//     /**
//      * @brief polygon retourne le triangle représentant la voiture
//      */
//     QGraphicsPolygonItem* polygon() const;

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
    // position de depart de la voiture
    QPointF d_from;
    // position de position d'arrivée
    QPointF d_to = {750, 400};
    // pixmap pour dessiner la voiture
    QGraphicsPixmapItem* d_pixmap = nullptr;
    double d_elapsed = 0.0; // Temps écoulé depuis le début de l'animation

//     // cercle pour représenter la fréquence de la voiture
//     QGraphicsEllipseItem* d_ellipse = nullptr;

    /**
    * @brief distance: calcule la distance entre la voiture et la position passée en paramètre
    * @return la distance obtenue
    */
    double distance() const;

    /**
    * @brief duree: retourne la durée pour parcourir une distance donnée
    * @param distance
    * @return la durée obtenue
    */
    double duree(double distance) const;
 };

#endif // CAR_H
