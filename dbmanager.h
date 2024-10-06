#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>

class DBManager
{
public:
    DBManager();
    QSqlDatabase getDbManager() const;

    /**
     * @brief getBounds: retourne la requete pour avoir les coordonnes max et min
     * @return {{MaxLon, MaxLat}, {MinLon, MinLat}}
     */
    QString getBounds() const;

    /**
     * @brief getNodes: retourne la requete pour recuperer les noeuds
     * @return la requete
     */
    QString getNodes() const;
    /**
     * @brief getNodeDs: retourne la requete pour recuperer les noeud descriptifs
     * @return la requete
     */
    QString getNodeDs() const;
    /**
     * @brief getRoads: retourne la requete pour recuperer routes
     * @return la requete
     */
    QString getRoads() const;
    /**
     * @brief getWaters: retourne la requete pour recuperer les eaux
     * @return la requete
     */
    QString getWaters() const;
    /**
     * @brief getBuildings: retourne la requete pour recuperer les immeubles
     * @return la requete
     */
    QString getBuildings() const;
    /**
     * @brief getParks: retourne la requete pour recuperer les espaces verts
     * @return la requete
     */
    QString getParks() const;
    /**
     * @brief getWayNodes: recuperer les noeuds d'un ways
     * @param id du noeuds
     * @return
     */
    QString getWayNodes(unsigned int) const;

protected:
    QSqlDatabase d_db;
};

#endif // DBMANAGER_H
