
#ifndef CAR_H
#define CAR_H

#include "node.h"
#include <QGraphicsPixmapItem>

class Car
{
public:
    // constructeur par defaut
    Car();
    /**
     * @brief Car avec parametre
     * @param pos: la position de la voiture
     * @param vitesse: la vitesse de la voiture
     * @param frequence: la frequence de la voiture
     */
    Car(const Node& pos, double vitesse = 50.0, double frequence = 1.5);
    /**
     * @brief Car construteur de recopie
     */
    Car(const Car&);

    /**
     * @brief moveTo deplace la voiture à la nouvelle position donnée
     */
    void moveTo(const Node&);

    QGraphicsPixmapItem* pixmap() const;

private:
    // vitesse de la voiture
    double d_v;
    // frequence
    double d_freq;
    // position de la voiture
    Node d_pos;
    // pixmap pour dessiner la voiture
    QGraphicsPixmapItem* d_pixmap = nullptr;
    // cercle pour representer la frequence de la voiture
    QGraphicsEllipseItem* d_ellipse = nullptr;

    /**
     * @brief distance: calcule la distance entre la voiture et la position passée en paramètre
     * @return la distance obtenu
     */
    double distance(const Node&) const;
    /**
     * @brief duree: retourne la durée pour parcourir une distance données
     * @param distance
     * @return la durée obtenu
     */
    double duree(double distance) const;
};

#endif // CAR_H
