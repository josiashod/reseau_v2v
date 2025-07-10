#ifndef _MAPITEM_H_
#define _MAPITEM_H_

#include <QString>
#include <vector>
#include <map>
#include <QGraphicsItemGroup>
#include <QBrush>
#include <QPen>
#include <QDebug>

class MapItem
{
public:

    MapItem(long long);

    MapItem(long long, const std::vector<QPointF>&);

    /**
     * @brief id
     * @return retourne l'id du MapItem
     */
    long long id() const;

    /**
     * @brief addPoint: ajoute un noeud à la liste des noeuds du mapItem
     * @param p: le point en question
     */
    void addPoint(const QPointF& p);
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
    bool tagExist(const QString& tag) const;
    /**
     * @brief tag: retourne la valeur du tag donné
     * @param tag: le nom du tag à retourner
     * @return QString la valeur du tag
     */
    QString tag(const QString& tag) const;
    /**
     * @brief draw: dessine la route avec un scale_factor donné
     * @param group le group ou doit etre l'instance
     * @param scale_factor: le scale factor
     */
    virtual void draw(QGraphicsItemGroup* group) const;
protected:
    long long d_id;
    std::vector<QPointF> d_points;
    std::map<QString, QString> d_tags;
};

#endif // MAPITEM_H
