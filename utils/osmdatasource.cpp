#include "OsmDataSource.h"
#include "../ui/widgets/logwidget.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

bool OsmDataSource::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LogWidget::addLog(QString("OsmDataSource: Cannot open file %1").arg(filePath), LogWidget::DANGER);
        return false;
    }

    QXmlStreamReader xml(&file);
    int nodeCount = 0, wayCount = 0;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            if (xml.name() == "bounds") {
                d_minLat = xml.attributes().value("minlat").toDouble();
                d_minLon = xml.attributes().value("minlon").toDouble();
                d_maxLat = xml.attributes().value("maxlat").toDouble();
                d_maxLon = xml.attributes().value("maxlon").toDouble();
                d_boundsLoaded = true;
                
                LogWidget::addLog(
                    QString("Bounds: [%1,%2] to [%3,%4]")
                    .arg(d_minLat).arg(d_minLon).arg(d_maxLat).arg(d_maxLon),
                    LogWidget::INFO
                );
            }
            else if (xml.name() == "node") {
                long long id = xml.attributes().value("id").toString().toLongLong();
                double lat = xml.attributes().value("lat").toDouble();
                double lon = xml.attributes().value("lon").toDouble();

                osm::NodeData node;
                node.id = id;
                node.lat = lat;
                node.lon = lon;

                d_nodes[id] = node;
                nodeCount++;

                // Skip to next node (tags inside nodes are ignored for now)
                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "node")) {
                    xml.readNext();
                }
            }
            else if (xml.name() == "way") {
                long long wayId = xml.attributes().value("id").toString().toLongLong();
                osm::WayData way;
                way.id = wayId;

                // Parse way nodes and tags
                while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "way")) {
                    xml.readNext();

                    if (xml.tokenType() == QXmlStreamReader::StartElement) {
                        if (xml.name() == "nd") {
                            long long nodeRef = xml.attributes().value("ref").toString().toLongLong();
                            way.nodeIds.push_back(nodeRef);
                        }
                        else if (xml.name() == "tag") {
                            QString key = xml.attributes().value("k").toString();
                            QString value = xml.attributes().value("v").toString();
                            way.tags[key] = value;
                        }
                    }
                }

                d_ways[wayId] = way;
                wayCount++;
            }
        }
    }

    file.close();

    if (xml.hasError()) {
        LogWidget::addLog(
            QString("OsmDataSource: XML error: %1").arg(xml.errorString()),
            LogWidget::DANGER
        );
        return false;
    }

    LogWidget::addLog(
        QString("OsmDataSource: Loaded %1 nodes, %2 ways")
        .arg(nodeCount).arg(wayCount),
        LogWidget::SUCCESS
    );

    return true;
}

void OsmDataSource::classifyWay(const osm::WayData& way)
{
    // Vérifier les tags pour classifier le way
    
    // ROADS: tag highway=*
    if (way.tags.find("highway") != way.tags.end()) {
        d_roads.insert(way.id);
        return;
    }
    
    // BUILDINGS: tag building=*
    if (way.tags.find("building") != way.tags.end()) {
        d_buildings.insert(way.id);
        return;
    }
    
    // PARKS: tag leisure=park ou landuse=grass/forest
    auto leisureIt = way.tags.find("leisure");
    if (leisureIt != way.tags.end() && leisureIt->second == "park") {
        d_parks.insert(way.id);
        return;
    }
    
    auto landuseIt = way.tags.find("landuse");
    if (landuseIt != way.tags.end() && 
        (landuseIt->second == "grass" || landuseIt->second == "forest" || landuseIt->second == "park")) {
        d_parks.insert(way.id);
        return;
    }
    
    // WATERS: tag natural=water ou waterway=*
    auto naturalIt = way.tags.find("natural");
    if (naturalIt != way.tags.end() && naturalIt->second == "water") {
        d_waters.insert(way.id);
        return;
    }
    
    auto waterwayIt = way.tags.find("waterway");
    if (waterwayIt != way.tags.end()) {
        d_waters.insert(way.id);
        return;
    }
}

bool OsmDataSource::isCarWay(const osm::WayData& way)
{
    // Vérifier si c'est une route carrossable
    auto it = way.tags.find("highway");
    if (it == way.tags.end()) {
        return false;
    }
    
    QString highway = it->second;
    
    // Routes carrossables (exclure footway, path, track, etc)
    static const std::set<QString> carableRoads = {
        "motorway", "trunk", "primary", "secondary", "tertiary",
        "unclassified", "residential", "service", "living_street"
    };
    
    return carableRoads.find(highway) != carableRoads.end();
}

std::vector<osm::NodeData> OsmDataSource::getNodesInBounds(
    double minLat, double minLon,
    double maxLat, double maxLon)
{
    std::vector<osm::NodeData> result;

    for (const auto& pair : d_nodes) {
        const auto& node = pair.second;
        if (node.lat >= minLat && node.lat <= maxLat &&
            node.lon >= minLon && node.lon <= maxLon) {
            result.push_back(node);
        }
    }

    return result;
}

std::vector<osm::WayData> OsmDataSource::getWaysInBounds(
    double minLat, double minLon,
    double maxLat, double maxLon)
{
    std::vector<osm::WayData> result;

    for (const auto& pair : d_ways) {
        const auto& way = pair.second;
        
        // Check if any node of this way is in bounds
        bool isInBounds = false;
        for (long long nodeId : way.nodeIds) {
            auto it = d_nodes.find(nodeId);
            if (it != d_nodes.end()) {
                const auto& node = it->second;
                if (node.lat >= minLat && node.lat <= maxLat &&
                    node.lon >= minLon && node.lon <= maxLon) {
                    isInBounds = true;
                    break;
                }
            }
        }

        if (isInBounds) {
            result.push_back(way);
        }
    }

    return result;
}

std::vector<osm::NodeData> OsmDataSource::getWayNodes(long long wayId)
{
    std::vector<osm::NodeData> result;

    auto it = d_ways.find(wayId);
    if (it != d_ways.end()) {
        const auto& way = it->second;
        
        for (long long nodeId : way.nodeIds) {
            auto nodeIt = d_nodes.find(nodeId);
            if (nodeIt != d_nodes.end()) {
                result.push_back(nodeIt->second);
            }
        }
    }

    return result;
}

osm::NodeData* OsmDataSource::getNodeById(long long id)
{
    auto it = d_nodes.find(id);
    if (it != d_nodes.end()) {
        return new osm::NodeData(it->second);
    }
    return nullptr;
}

osm::WayData* OsmDataSource::getWayById(long long id)
{
    auto it = d_ways.find(id);
    if (it != d_ways.end()) {
        return new osm::WayData(it->second);
    }
    return nullptr;
}

void OsmDataSource::getBounds(
    double& minLat, double& minLon,
    double& maxLat, double& maxLon) const
{
    minLat = d_minLat;
    minLon = d_minLon;
    maxLat = d_maxLat;
    maxLon = d_maxLon;
}

size_t OsmDataSource::nodeCount() const
{
    return d_nodes.size();
}

size_t OsmDataSource::wayCount() const
{
    return d_ways.size();
}

std::vector<osm::WayData> OsmDataSource::getAllWays()
{
    std::vector<osm::WayData> result;
    
    for (const auto& pair : d_ways) {
        result.push_back(pair.second);
    }
    
    return result;
}
