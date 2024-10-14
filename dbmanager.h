#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QThread>
#include <QHash>

class DBManager
{
public:
    ~DBManager();
    /**
     * @brief getInstance retourne l'instance unique d'utilisation de la bd
     * @return
     */
    static DBManager* getInstance();
    /**
     * @brief getDatabase retourne la base de donnée du thread en cours
     * @return
     */
    QSqlDatabase getDatabase();
    /**
     * @brief destroyInstance Suppression de l'instance
     */
    static void destroyInstance();
    /**
     * @brief getBounds: retourne la requete pour avoir les coordonnes max et min
     * @return {{MaxLon, MaxLat}, {MinLon, MinLat}}
     */
    QSqlQuery getBounds(QSqlDatabase db) const;

    /**
     * @brief getNodes: retourne la requete pour recuperer les noeuds
     * @return la requete
     */
    QSqlQuery getNodes(QSqlDatabase db) const;
    /**
     * @brief getNodeDs: retourne la requete pour recuperer les noeud descriptifs
     * @return la requete
     */
    QSqlQuery getNodeDs(QSqlDatabase db) const;
    /**
     * @brief getRoads: retourne la requete pour recuperer routes
     * @return la requete
     */
    QSqlQuery getRoads(QSqlDatabase db) const;
    /**
     * @brief getWaters: retourne la requete pour recuperer les eaux
     * @return la requete
     */
    QSqlQuery getWaters(QSqlDatabase db) const;
    /**
     * @brief getBuildings: retourne la requete pour recuperer les immeubles
     * @return la requete
     */
    QSqlQuery getBuildings(QSqlDatabase db) const;
    /**
     * @brief getParks: retourne la requete pour recuperer les espaces verts
     * @return la requete
     */
    QSqlQuery getParks(QSqlDatabase db) const;
    /**
     * @brief getWayNodes: recuperer les noeuds d'un ways
     * @param id du noeuds
     * @return
     */
    QSqlQuery getWayNodes(QSqlDatabase db, long long id) const;

private:
    DBManager();
    static DBManager* d_instance;
};

#endif // DBMANAGER_H
