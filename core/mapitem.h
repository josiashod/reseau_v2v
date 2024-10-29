#ifndef _MAPITEM_H_
#define _MAPITEM_H_

#include <QString>
#include <vector>
#include <map>
#include <QGraphicsItemGroup>
#include <QBrush>
#include <QPen>
#include "node.h"

class MapItem
{
public:

    MapItem(long long);

    /**
     * @brief id
     * @return retourne l'id du MapItem
     */
    long long id() const;

    /**
     * @brief addNode: ajoute un noeud à la liste des noeuds du mapItem
     * @param n: le noeud en question
     */
    void addNode(const Node& n);
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
     * @brief draw: dessine la route avec un scale_factor donné
     * @param group le group ou doit etre l'instance
     * @param scale_factor: le scale factor
     */
    virtual void draw(QGraphicsItemGroup* group, double scale_factor) const = 0;
protected:
    long long d_id;
    std::vector<Node> d_nodes;
    std::map<QString, QString> d_tags;
};

#endif // MAPITEM_H
