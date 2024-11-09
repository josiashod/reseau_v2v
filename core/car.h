#ifndef CAR_H
#define CAR_H

#include "node.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>

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
    Car(const Node& pos, double vitesse = 50.0, double frequence = 1.5);

    /**
     * @brief Car constructeur de recopie
     */
    Car(const Car&);

    /**
     * @brief moveTo déplace la voiture à la nouvelle position donnée
     */
    void moveTo(const Node&);

    /**
     * @brief pixmap retourne l'image de la voiture (si utilisée)
     */
    QGraphicsPixmapItem* pixmap() const;

    /**
     * @brief polygon retourne le triangle représentant la voiture
     */
    QGraphicsPolygonItem* polygon() const;

private:
    // vitesse de la voiture
    double d_v;
    // fréquence
    double d_freq;
    // position de la voiture
    Node d_pos;
    // pixmap pour dessiner la voiture
    QGraphicsPixmapItem* d_pixmap = nullptr;
    // triangle pour représenter la voiture
    QGraphicsPolygonItem* d_polygon = nullptr;
    // cercle pour représenter la fréquence de la voiture
    QGraphicsEllipseItem* d_ellipse = nullptr;

    /**
     * @brief distance: calcule la distance entre la voiture et la position passée en paramètre
     * @return la distance obtenue
     */
    double distance(const Node&) const;

    /**
     * @brief duree: retourne la durée pour parcourir une distance donnée
     * @param distance
     * @return la durée obtenue
     */
    double duree(double distance) const;
};

#endif // CAR_H
