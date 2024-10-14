#ifndef BUILDING_H
#define BUILDING_H

#include "mapitem.h"

class Building : public MapItem
{
public:
    Building(long long id);

    /**
     * @brief draw: dessine la route avec un scale_factor donné
     * @param group le group ou doit etre l'instance
     * @param scale_factor: le scale factor
     */
    void draw(QGraphicsItemGroup* group, double scale_factor) const override;
};

#endif // BUILDING_H
