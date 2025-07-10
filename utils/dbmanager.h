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
    /**
     * @brief getInstance retourne l'instance unique d'utilisation de la bd
     * @return
     */
   static DBManager& instance();
    /**
     * @brief retourne la base de donnée du thread en cours
     * @return
     */
    QSqlDatabase& database();

   /**
     * @brief create database by threads
     * @return
     */
   static QSqlDatabase threadDatabase();

    /**
     * @brief close database
     */
    void close();
    /**
     * @brief getBounds: retourne la requete pour avoir les coordonnes max et min
     * @return {{MaxLon, MaxLat}, {MinLon, MinLat}}
     */
    QSqlQuery getBounds() const;

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
    ~DBManager();
    QSqlDatabase d_db;
    /**
     * @brief tel if database is open or not
     */
    bool d_open;
    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;
};

#endif // DBMANAGER_H
