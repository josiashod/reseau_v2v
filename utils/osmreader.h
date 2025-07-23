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
    static void readNode(QXmlStreamReader& xml, int& nodeCount, int& tagCount);
    static void readWay(QXmlStreamReader& xml, int& nodeCount, int& tagCount);
};

#endif // OSMREADER_H
