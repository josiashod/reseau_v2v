#include "dbmanager.h"
#include "../env.h"
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
const QString _BOUNDS_TABLE_ = "bounds";

// Initialisation des attributs static
//DBManager* DBManager::d_instance = nullptr;
size_t DBManager::d_compteur_instance = 0;

DBManager::DBManager()
{

    d_db = QSqlDatabase::addDatabase(env::DB_CONNECTION, QString("opening_database_%1").arg(d_compteur_instance++));

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
    }
    else
    {
      qDebug() << "Database: connection ok";
    }
}

DBManager::~DBManager()
{
    // if (d_db.isOpen())
    // {
    //     d_db.close();
    //     qDebug() << "Database: connection closed";
    // }
    // QSqlDatabase::removeDatabase(d_db.connectionName());

    if (!QSqlDatabase::database(d_db.connectionName(), false).isOpen()) {
        d_db.close();
        QSqlDatabase::removeDatabase(d_db.connectionName());
        qDebug() << "Database: connection closed";
    }

//    QString threadId = QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));
//    if (QSqlDatabase::contains(threadId)) {
//        qDebug() << threadId;
//        QSqlDatabase db = QSqlDatabase::database(threadId);
//        if (db.isOpen()) {
//            db.close(); // Close the database connection
//        }
//        QSqlDatabase::removeDatabase(threadId);
//        QSqlDatabase::removeDatabase(db.connectionName());
//    }
}

//DBManager* DBManager::getInstance()
//{
//    if (d_instance == nullptr) {
//        d_instance = new DBManager();
//    }
//    return d_instance;
//}

//void DBManager::destroyInstance()
//{
//    if(d_instance)
//        delete d_instance;
//    d_instance = nullptr;
//}

//void DBManager::closeDatabase()
//{
//    if (d_db.isOpen())
//    {
//        d_db.close();
//        qDebug() << "Database: connection closed";
//    }
//}

QSqlDatabase DBManager::getDatabase()
{
//    // Obtenez le chemin vers un répertoire accessible en lecture et écriture (par exemple un répertoire temporaire)
//    QString directory = QDir::homePath();
//    directory += "/" + QStandardPaths::displayName(QStandardPaths::DocumentsLocation) + "/";
//    // Créez le répertoire si nécessaire
//    QDir dir(directory);
//    if (!dir.exists()) {
//        dir.mkpath(".");
//    }
//    // Chemin complet où la base de données sera copiée
//    QString dbPath = directory + "mulhouse_network/geo.db";
//    // Si la base de données n'existe pas déjà à cet emplacement, copiez-la depuis les ressources
//    if (!QFile::exists(dbPath))
//        qDebug() << "Error: Unable to find database file: " << dbPath;

//    QString threadId = QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));
//    if (!QSqlDatabase::contains(threadId)) {
//        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", threadId);

//        db.setDatabaseName(dbPath);

//       if (!db.open())
//       {
//           qDebug() << "Error: Unable to open database";
//       }
//       else
//       {
//          qDebug() << "Database: connection ok";
//       }
//    }
//    return QSqlDatabase::database(threadId);
    return d_db;
}

QSqlQuery DBManager::getBounds(QSqlDatabase db) const
{
    QString queryStr = QString("SELECT * FROM %1").arg(_BOUNDS_TABLE_);
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
