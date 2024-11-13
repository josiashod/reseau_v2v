#ifndef PARK_H
#define PARK_H

#include "mapitem.h"

class Park : public MapItem
{
public:
    Park(long long id);

    /**
     * @brief draw: dessine la route avec un scale_factor donné
     * @param group le group ou doit etre l'instance
     * @param scale_factor: le scale factor
     */
    void draw(QGraphicsItemGroup* group) const override;
};

#endif // PARK_H
