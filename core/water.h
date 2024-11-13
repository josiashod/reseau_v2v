#ifndef WATER_H
#define WATER_H

#include "mapitem.h"

class Water : public MapItem
{
public:
    Water(long long id);

    /**
     * @brief draw: dessine la route avec un scale_factor donné
     * @param group le group ou doit etre l'instance
     * @param scale_factor: le scale factor
     */
    void draw(QGraphicsItemGroup* group) const override;
};

#endif // WATER_H
