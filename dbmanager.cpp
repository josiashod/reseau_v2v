#include "dbmanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QFile>
#include <QFileInfo>

const QString _NODES_TABLE_ = "nodes";
const QString _WAYS_TABLE_ = "ways";
const QString _WAY_NODE_TABLE_ = "way_node";
const QString _TAGS_TABLE_ = "ways";

DBManager::DBManager()
{
    d_db = QSqlDatabase::addDatabase("QSQLITE");
    d_db.setDatabaseName("/home/josh/projects/QtProjects/mulhouse_network/lib/geo.db");

   if (!d_db.open())
   {
       qDebug() << "Error: Unable to open database" << d_db.lastError().text();
   }
   else
   {
      qDebug() << "Database: connection ok";
   }
}

QSqlDatabase DBManager::getDbManager() const
{ return d_db; }

std::pair<std::pair<double, double>, std::pair<double, double>> DBManager::getBounds() const
{
    double minLat = 0.0, maxLat = 0.0, minLon = 0.0, maxLon = 0.0;
    QSqlQuery query;

    bool success = query.exec("SELECT min(lat) as min_lat, max(lat) as max_lat, min(lon) as min_lon, max(lon) as max_lon from nodes");
    if(success)
    {
        if(query.next())
        {
            minLat = query.value(0).toString().toFloat();
            maxLat = query.value(1).toString().toFloat();
            minLon = query.value(2).toString().toFloat();
            maxLon = query.value(3).toString().toFloat();
        }
    }

    return {{maxLon, maxLat}, {minLon, minLat}};
}
