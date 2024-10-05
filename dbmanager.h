#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>

#include "mapitem.h"
#include "node.h"
#include "noded.h"
#include "way.h"
#include "building.h"
#include "water.h"
#include "parc.h"

class DBManager
{
public:
    DBManager();
    QSqlDatabase getDbManager() const;

    /**
     * @brief getBounds: retourne le coordonnes max et min
     * @return {{MaxLon, MaxLat}, {MinLon, MinLat}}
     */
    std::pair<std::pair<double, double>, std::pair<double, double>> getBounds() const;
protected:
    QSqlDatabase d_db;
};

#endif // DBMANAGER_H
