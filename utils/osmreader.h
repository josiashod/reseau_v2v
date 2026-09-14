#ifndef OSMREADER_H
#define OSMREADER_H

#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QString>
#include <map>
#include <unordered_map>
#include <vector>

struct OsmBounds
{
    double minLat = 0.0;
    double minLon = 0.0;
    double maxLat = 0.0;
    double maxLon = 0.0;
    bool valid = false;
};

struct OsmFeature
{
    long long id = 0;
    QString type;
    QString subtype;
    std::vector<long long> nodeRefs;
    std::map<QString, QString> tags;
};

struct ParsedOsmMap
{
    OsmBounds bounds;
    std::unordered_map<long long, std::pair<double, double>> nodes; // id -> lon, lat
    std::vector<OsmFeature> roads;
    std::vector<OsmFeature> buildings;
    std::vector<OsmFeature> waters;
    std::vector<OsmFeature> parks;
};

class OsmReader
{
public:
    OsmReader();
    static ParsedOsmMap parseMapFile(const QString& filePath);
    static void readOSMFile(const QString& filePath);
private:
    static void readBounds(QXmlStreamReader& xml);
    static void readNode(QXmlStreamReader& xml, int& nodeCount, int& tagCount);
    static void readWay(QXmlStreamReader& xml, int& nodeCount, int& tagCount);
};

#endif // OSMREADER_H
