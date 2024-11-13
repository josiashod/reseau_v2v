#ifndef WAY_H
#define WAY_H

#include "mapitem.h"

class Way: public MapItem
{
public:
    Way(): MapItem{-1}
    {}

    Way(long long id);

    /**
     * @brief draw: dessine la route avec un scale_factor donné
     * @param group le group ou doit etre l'instance
     * @param scale_factor: le scale factor
     */
    void draw(QGraphicsItemGroup* group) const override;

private:
    long long d_id;
};

#endif // WAY_H
