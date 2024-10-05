#ifndef WAY_H
#define WAY_H

#include "mapitem.h"

class Way: public MapItem
{
public:
    Way(unsigned int id);

//    /**
//     * @brief draw: dessine la route avec un scale_factor donné
//     * @param scale_factor: le scale factor
//     */
//    void draw(double scale_factor) const override;
};

#endif // WAY_H
