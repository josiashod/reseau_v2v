#ifndef WAY_H
#define WAY_H

#include "mapitem.h"

class Way: public MapItem
{
public:
    Way(long long id);

    /**
     * @brief getNodeIndex: retourne l'index du noeud dans la liste de noeud du mapItem
     * @param id: l'id du noeud a trouvé
     * @return -1 s'il n'est pas trouvé sinon retourne l'index
     */
    long long getNodeIndex(const long long id);
    /**
     * @brief getNode: retourne le noeud
     * @param id: l'id du noeud a trouvé
     * @return
     */
    Node& getNode(long long index) const;

    /**
     * @brief nodeSize: retourne le nombre de node du mapItem
     * @return la taille des nodes
     */
    unsigned int nodeSize() const;

    /**
     * @brief draw: dessine la route avec un scale_factor donné
     * @param group le group ou doit etre l'instance
     * @param scale_factor: le scale factor
     */
    void draw(QGraphicsItemGroup* group, double scale_factor) const override;
    static Way* findWayById(long long id); // Méthode statique pour trouver un `way` par son ID
        QVector<Node> nodes() const; // Retourne tous les nœuds associés à ce `way`
    void addNode(const Node& node);

private:
    long long d_id;
    QVector<Node> d_nodes;
};

#endif // WAY_H
