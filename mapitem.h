#ifndef _MAPITEM_H_
#define _MAPITEM_H_

#include <QString>
#include <vector>
#include <map>
#include "node.h"

class MapItem
{
public:

    MapItem(unsigned int);

    /**
     * @brief addNode: ajoute un noeud à la liste des noeuds du mapItem
     * @param n: le noeud en question
     */
    void addNode(Node* n);
    /**
     * @brief addTag: ajoute un tag a
     * @param key
     * @param value
     */
    void addTag(const QString& key, const QString &value);
    /**
     * @brief tagExist: vérifie si le tag donné existe dans la liste des tags
     * @param tag: le nom du tag à trouver
     * @return true si le tag existe false sinon
     */
    bool tagExist(QString tag) const;
    /**
     * @brief tag: retourne la valeur du tag donné
     * @param tag: le nom du tag à retourner
     * @return QString la valeur du tag
     */
    QString tag(QString tag) const;
    /**
     * @brief nodeSize: retourne le nombre de node du mapItem
     * @return la taille des nodes
     */
    unsigned int nodeSize() const;
//    /**
//     * @brief draw: dessine le mapItem en fonction un scale_factor donné
//     * @param scale_factor: le scale factor
//     */
//    virtual void draw(double scale_factor) const = 0;
protected:
    unsigned int d_id;
    std::vector<Node*> d_nodes;
    std::map<QString, QString> d_tags;
};

#endif // MAPITEM_H
