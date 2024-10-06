#include "dbmanager.h"
#include <QDebug>
#include <QSqlRecord>
#include <QFile>
#include <QFileInfo>
#include "mapviewer.h"

const QString _NODES_TABLE_ = "nodes";
const QString _WAYS_TABLE_ = "ways";
const QString _WAY_NODE_TABLE_ = "way_node";
const QString _TAGS_TABLE_ = "tags";

DBManager::DBManager()
{
    d_db = QSqlDatabase::addDatabase("QSQLITE");
    d_db.setDatabaseName("/home/josh/projects/QtProjects/mulhouse_network/lib/geo.db");

   if (!d_db.open())
   {
       qDebug() << "Error: Unable to open database";
   }
   else
   {
      qDebug() << "Database: connection ok";
   }
}

QSqlDatabase DBManager::getDbManager() const
{ return d_db; }

QString  DBManager::getBounds() const
{
    QString queryStr = QString("SELECT min(lat) as min_lat, max(lat) as max_lat, min(lon) as min_lon, max(lon) as max_lon FROM %1").arg(_NODES_TABLE_);

    return queryStr;
}

QString DBManager::getNodes() const
{
    QString queryStr = QString("SELECT n.id, n.lat, n.lon "
    "FROM %1 AS n "
    "LEFT JOIN %2 AS wn ON wn.node_id = n.id "
    "LEFT JOIN %3 AS t ON t.element_id = n.id "
    "WHERE wn.way_id IS NOT NULL AND t.element_id IS NULL").arg(_NODES_TABLE_, _WAY_NODE_TABLE_, _TAGS_TABLE_);
    return queryStr;
}

QString DBManager::getNodeDs() const
{
    QString queryStr = QString("SELECT  * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
       "WHERE t.key = '%3'").arg(_NODES_TABLE_, _TAGS_TABLE_, "name");

    return queryStr;
}

QString DBManager::getRoads() const
{
    QString queryStr = QString("SELECT  * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
        "WHERE t.key = 'highway'").arg(_WAYS_TABLE_, _TAGS_TABLE_);
    return queryStr;
}

QString DBManager::getWaters() const
{
    QString queryStr = QString("SELECT  * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
        "WHERE t.key = 'waterway'").arg(_WAYS_TABLE_, _TAGS_TABLE_);
    return queryStr;
}

QString DBManager::getBuildings() const
{
    QString queryStr = QString("SELECT  * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
        "WHERE t.key = 'buildings'").arg(_WAYS_TABLE_, _TAGS_TABLE_);
    return queryStr;
}

QString DBManager::getParks() const
{
    QString queryStr = QString("SELECT  * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
        "WHERE t.key = 'leisure' and t.value IN ('garden', 'nature_reserve', 'playground', 'nature_reserve')").arg(_WAYS_TABLE_, _TAGS_TABLE_);
    return queryStr;
}

QString DBManager::getWayNodes(unsigned int id) const
{
    QString queryStr = QString("SELECT node_id "
        "FROM %1 "
        "WHERE way_id = %2").arg(_WAY_NODE_TABLE_, id);
    return queryStr;
}
