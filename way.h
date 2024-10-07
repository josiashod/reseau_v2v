#ifndef WAY_H
#define WAY_H

#include "mapitem.h"

class Way: public MapItem
{
public:
    Way(unsigned int id);

    /**
     * @brief getNodeIndex: retourne l'index du noeud dans la liste de noeud du mapItem
     * @param id: l'id du noeud a trouvé
     * @return -1 s'il n'est pas trouvé sinon retourne l'index
     */
    unsigned int getNodeIndex(const unsigned int id);
    /**
     * @brief getNode: retourne le noeud
     * @param id: l'id du noeud a trouvé
     * @return
     */
    Node& getNode(unsigned int index) const;

//    /**
//     * @brief draw: dessine la route avec un scale_factor donné
//     * @param scale_factor: le scale factor
//     */
//    void draw(double scale_factor) const override;
};

#endif // WAY_H
