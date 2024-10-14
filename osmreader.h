#ifndef OSMREADER_H
#define OSMREADER_H

#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include "dbmanager.h"

class OsmReader
{
public:
    OsmReader();
    void readOSMFile(const QString& filePath);
private:
    void readNode(QXmlStreamReader& xml);
//    void readRelation(QXmlStreamReader& xml);
    void readWay(QXmlStreamReader& xml);

//    DBManager d_db;
};

#endif // OSMREADER_H
