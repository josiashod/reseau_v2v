#ifndef OSMREADER_H
#define OSMREADER_H

#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

class OsmReader
{
public:
    OsmReader();
    static void readOSMFile(const QString& filePath);
private:
    static void readBounds(QXmlStreamReader& xml);
    static void readNode(QXmlStreamReader& xml);
    void readRelation(QXmlStreamReader& xml);
    static void readWay(QXmlStreamReader& xml);
};

#endif // OSMREADER_H
