#include "dbmanager.h"
#include <QDebug>
#include <QSqlRecord>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

const QString _NODES_TABLE_ = "nodes";
const QString _WAYS_TABLE_ = "ways";
const QString _WAY_NODE_TABLE_ = "way_node";
const QString _TAGS_TABLE_ = "tags";

// Initialisation de l'instance statique à nullptr
DBManager* DBManager::d_instance = nullptr;

DBManager::DBManager()
{}

DBManager::~DBManager()
{
    QString threadId = QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    if (QSqlDatabase::contains(threadId)) {
        qDebug() << threadId;
        QSqlDatabase db = QSqlDatabase::database(threadId);
        if (db.isOpen()) {
            db.close(); // Close the database connection
        }
        QSqlDatabase::removeDatabase(threadId);
        QSqlDatabase::removeDatabase(db.connectionName());
    }

//    if (d_db.isOpen())
//    {
//        d_db.close();
//        qDebug() << "Database: connection closed";
//    }
}

DBManager* DBManager::getInstance()
{
    if (d_instance == nullptr) {
        d_instance = new DBManager();
    }
    return d_instance;
}

void DBManager::destroyInstance()
{
    if(d_instance)
        delete d_instance;
    d_instance = nullptr;
}

QSqlDatabase DBManager::getDatabase()
{
    // Obtenez le chemin vers un répertoire accessible en lecture et écriture (par exemple un répertoire temporaire)
    QString directory = QDir::homePath();
    directory += "/" + QStandardPaths::displayName(QStandardPaths::DocumentsLocation) + "/";
    // Créez le répertoire si nécessaire
    QDir dir(directory);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    // Chemin complet où la base de données sera copiée
    QString dbPath = directory + "mulhouse_network/geo.db";
    // Si la base de données n'existe pas déjà à cet emplacement, copiez-la depuis les ressources
    if (!QFile::exists(dbPath))
        qDebug() << "Error: Unable to find database file: " << dbPath;

    QString threadId = QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));
    if (!QSqlDatabase::contains(threadId)) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", threadId);

        db.setDatabaseName(dbPath);

       if (!db.open())
       {
           qDebug() << "Error: Unable to open database";
       }
       else
       {
          qDebug() << "Database: connection ok";
       }
    }
    return QSqlDatabase::database(threadId);
}

QSqlQuery DBManager::getBounds(QSqlDatabase db) const
{
    QString queryStr = QString("SELECT min(lat) as min_lat, max(lat) as max_lat, min(lon) as min_lon, max(lon) as max_lon FROM %1").arg(_NODES_TABLE_);
    QSqlQuery q(db);
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
        "WHERE t.t_key = 'waterway' LIMIT 10").arg(_WAYS_TABLE_, _TAGS_TABLE_);
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
long long DBManager::getRandomWay() {
    auto db = getInstance()->getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT id FROM ways ORDER BY RANDOM() LIMIT 1");

    if (query.exec() && query.next()) {
        return query.value(0).toLongLong(); // Retourne l'ID du `way` aléatoire
    } else {
        qDebug() << "Erreur lors de la récupération d'un way aléatoire : " << query.lastError().text();
        return -1; // Retourne -1 en cas d'erreur
    }
}
QVector<long long> DBManager::getWaysByNode(long long nodeId) {
    QVector<long long> wayIds;
    auto db = getInstance()->getDatabase();
    QSqlQuery query(db);
    query.prepare("SELECT way_id FROM way_node WHERE node_id = :node_id");
    query.bindValue(":node_id", nodeId);

    if (query.exec()) {
        while (query.next()) {
            wayIds.append(query.value(0).toLongLong());
        }
    } else {
        qDebug() << "Erreur lors de la récupération des ways pour le nœud : " << query.lastError().text();
    }

    return wayIds;
}
