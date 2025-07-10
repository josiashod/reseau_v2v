#include "dbmanager.h"
#include "../env.h"
#include <QDebug>
#include <QSqlRecord>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

static constexpr const char* _NODES_TABLE_ = "nodes";
static constexpr const char* _WAYS_TABLE_ = "ways";
static constexpr const char* _WAY_NODE_TABLE_ = "way_node";
static constexpr const char* _TAGS_TABLE_ = "tags";
static constexpr const char* _BOUNDS_TABLE_ = "bounds";

DBManager::DBManager(): d_open{false}
{
    d_db = QSqlDatabase::addDatabase(env::DB_CONNECTION);

    if(env::DB_CONNECTION == "QSQLITE")
    {
        d_db.setDatabaseName(env::DB_DATABASE);
    }
    else
    {
        d_db.setHostName(env::DB_HOST);
        d_db.setPort(env::DB_PORT);
        d_db.setDatabaseName(env::DB_DATABASE);
        d_db.setUserName(env::DB_USERNAME);
        d_db.setPassword(env::DB_PASSWORD);
    }

    if (!d_db.open())
    {
       qDebug() << "Error: Unable to open database";
        d_open = true;
    }
    else
    {
      qDebug() << "Database: connection ok";
    }
}

DBManager::~DBManager()
{
    close();
}

QSqlDatabase DBManager::threadDatabase()
{
    QString connectionName = QString::number((quintptr)QThread::currentThreadId());
    if (!QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db = QSqlDatabase::addDatabase(env::DB_CONNECTION, connectionName);
        if(env::DB_CONNECTION == "QSQLITE") {
            db.setDatabaseName(env::DB_DATABASE);
        } else {
            db.setHostName(env::DB_HOST);
            db.setPort(env::DB_PORT);
            db.setDatabaseName(env::DB_DATABASE);
            db.setUserName(env::DB_USERNAME);
            db.setPassword(env::DB_PASSWORD);
        }
        if (!db.open()) {
            qDebug() << "[threadDatabase] Error: Unable to open database in thread" << connectionName << db.lastError().text();
        } else {
            qDebug() << "[threadDatabase] Database opened for thread" << connectionName;
        }
    }
    return QSqlDatabase::database(connectionName);
}

DBManager& DBManager::instance()
{
    static DBManager d_instance;
    return d_instance;
}

void DBManager::close()
{
   if (d_db.isOpen())
   {
       d_db.close();
       d_open = false;
       qDebug() << "Database: connection closed";
   }
}

QSqlDatabase& DBManager::database()
{
    return d_db;
}

QSqlQuery DBManager::getBounds() const
{
    QString queryStr = QString("SELECT * FROM %1").arg(_BOUNDS_TABLE_);
    QSqlQuery q(instance().database());
    if (!q.prepare(queryStr)) {
        qDebug() << "Error preparing query: " << q.lastError().text();
    }

    return q;
}

QSqlQuery DBManager::getNodes(QSqlDatabase db) const
{
    QString queryStr = QString("SELECT n.id, n.lat, n.lon "
    "FROM %1 AS n "
    "LEFT JOIN %2 AS wn ON wn.node_id = n.id "
    "LEFT JOIN %3 AS t ON t.element_id = n.id "
    "WHERE wn.way_id IS NOT NULL AND t.element_id IS NULL").arg(_NODES_TABLE_, _WAY_NODE_TABLE_, _TAGS_TABLE_);
    QSqlQuery q(db);
    if (!q.prepare(queryStr)) {
        qDebug() << "Error preparing query: " << q.lastError().text();
    }

    return q;
}

QSqlQuery DBManager::getNodeDs(QSqlDatabase db) const
{
    QString queryStr = QString("SELECT  * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
       "WHERE t.t_key = '%3'").arg(_NODES_TABLE_, _TAGS_TABLE_, "name");
    QSqlQuery q(db);
    if (!q.prepare(queryStr)) {
        qDebug() << "Error preparing query: " << q.lastError().text();
    }

    return q;
}

QSqlQuery DBManager::getRoads(QSqlDatabase db) const
{
    QString queryStr = QString("SELECT  * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
        "WHERE t.t_key = 'highway'").arg(_WAYS_TABLE_, _TAGS_TABLE_);
    QSqlQuery q(db);
    if (!q.prepare(queryStr)) {
        qDebug() << "Error preparing query: " << q.lastError().text();
    }

    return q;
}

QSqlQuery DBManager::getWaters(QSqlDatabase db) const
{
    QString queryStr = QString("SELECT  * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
        "WHERE t.t_key = 'waterway' and t.t_value IN ('river', 'stream', 'canal')").arg(_WAYS_TABLE_, _TAGS_TABLE_);
    QSqlQuery q(db);
    if (!q.prepare(queryStr)) {
        qDebug() << "Error preparing query: " << q.lastError().text();
    }

    return q;
}

QSqlQuery DBManager::getBuildings(QSqlDatabase db) const
{
    QString queryStr = QString("SELECT * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
        "WHERE t.t_key = 'building'").arg(_WAYS_TABLE_, _TAGS_TABLE_);

    QSqlQuery q(db);
    if (!q.prepare(queryStr)) {
        qDebug() << "Error preparing query: " << q.lastError().text();
    }

    return q;
}

QSqlQuery DBManager::getParks(QSqlDatabase db) const
{
    QString queryStr = QString("SELECT * "
        "FROM %1 "
        "LEFT JOIN %2 as t ON t.element_id = %1.id "
        "WHERE (t.t_key = 'leisure' AND t.t_value IN ('garden', 'nature_reserve', 'playground', 'nature_reserve')) "
        "OR (t.t_key = 'landuse' AND t.t_value = 'grass') ").arg(_WAYS_TABLE_, _TAGS_TABLE_);
    QSqlQuery q(db);
    if (!q.prepare(queryStr)) {
        qDebug() << "Error preparing query: " << q.lastError().text();
    }

    return q;
}

QSqlQuery DBManager::getWayNodes(QSqlDatabase db, long long id) const
{
    QString queryStr = QString("SELECT DISTINCT n.id, n.lat, n.lon "
        "FROM %1 as wn "
        "LEFT JOIN %2 as n ON wn.node_id = n.id "
        "WHERE way_id = %3").arg(_WAY_NODE_TABLE_, _NODES_TABLE_, QString::number(id));
    QSqlQuery q(db);
    if (!q.prepare(queryStr)) {
        qDebug() << "Error preparing query: " << q.lastError().text();
    }

    return q;
}
