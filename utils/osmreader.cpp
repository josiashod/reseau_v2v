#include "osmreader.h"
#include "../ui/widgets/logwidget.h"
#include <QSqlQuery>
#include <algorithm>
#include <limits>
#include <utility>

OsmReader::OsmReader()
{}

namespace
{
QString tagValue(const std::map<QString, QString>& tags, const QString& key)
{
    auto it = tags.find(key);
    if(it == tags.end())
        return {};

    return it->second;
}

bool hasTag(const std::map<QString, QString>& tags, const QString& key)
{
    return tags.find(key) != tags.end();
}

bool hasAnyValue(const std::map<QString, QString>& tags, const QString& key, const std::vector<QString>& values)
{
    const QString value = tagValue(tags, key);
    for(const auto& candidate: values) {
        if(value == candidate)
            return true;
    }
    return false;
}

void updateBoundsFromNode(OsmBounds& bounds, double lat, double lon)
{
    if(!bounds.valid) {
        bounds.minLat = bounds.maxLat = lat;
        bounds.minLon = bounds.maxLon = lon;
        bounds.valid = true;
        return;
    }

    bounds.minLat = std::min(bounds.minLat, lat);
    bounds.maxLat = std::max(bounds.maxLat, lat);
    bounds.minLon = std::min(bounds.minLon, lon);
    bounds.maxLon = std::max(bounds.maxLon, lon);
}

void classifyFeature(ParsedOsmMap& map, OsmFeature&& feature)
{
    if(feature.nodeRefs.empty())
        return;

    if(hasTag(feature.tags, "highway")) {
        feature.type = "road";
        feature.subtype = tagValue(feature.tags, "highway");
        map.roads.push_back(std::move(feature));
        return;
    }

    if(hasTag(feature.tags, "building")) {
        feature.type = "building";
        feature.subtype = tagValue(feature.tags, "building");
        map.buildings.push_back(std::move(feature));
        return;
    }

    if(hasAnyValue(feature.tags, "waterway", {"river", "stream", "canal"}) ||
       hasAnyValue(feature.tags, "natural", {"water"}) ||
       hasAnyValue(feature.tags, "water", {"river", "stream", "canal", "lake", "pond"})) {
        feature.type = "water";
        feature.subtype = hasTag(feature.tags, "waterway") ? tagValue(feature.tags, "waterway") : tagValue(feature.tags, "natural");
        map.waters.push_back(std::move(feature));
        return;
    }

    if(hasAnyValue(feature.tags, "leisure", {"garden", "park", "nature_reserve", "playground"}) ||
       hasAnyValue(feature.tags, "landuse", {"grass", "forest", "meadow"})) {
        feature.type = "park";
        feature.subtype = hasTag(feature.tags, "leisure") ? tagValue(feature.tags, "leisure") : tagValue(feature.tags, "landuse");
        map.parks.push_back(std::move(feature));
    }
}
}

ParsedOsmMap OsmReader::parseMapFile(const QString& filePath)
{
    ParsedOsmMap map;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LogWidget::addLog(QString("Unable to open OSM file: %1").arg(filePath), LogWidget::DANGER);
        return map;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError()) {
        const QXmlStreamReader::TokenType token = xml.readNext();

        if(token != QXmlStreamReader::StartElement)
            continue;

        if(xml.name() == "bounds") {
            map.bounds.minLat = xml.attributes().value("minlat").toDouble();
            map.bounds.minLon = xml.attributes().value("minlon").toDouble();
            map.bounds.maxLat = xml.attributes().value("maxlat").toDouble();
            map.bounds.maxLon = xml.attributes().value("maxlon").toDouble();
            map.bounds.valid = true;
        }
        else if(xml.name() == "node") {
            const long long id = xml.attributes().value("id").toLongLong();
            const double lat = xml.attributes().value("lat").toDouble();
            const double lon = xml.attributes().value("lon").toDouble();
            map.nodes[id] = std::make_pair(lon, lat);

            if(!map.bounds.valid)
                updateBoundsFromNode(map.bounds, lat, lon);
        }
        else if(xml.name() == "way") {
            OsmFeature feature;
            feature.id = xml.attributes().value("id").toLongLong();

            while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "way") &&
                   !xml.atEnd() && !xml.hasError()) {
                xml.readNext();

                if(xml.tokenType() != QXmlStreamReader::StartElement)
                    continue;

                if(xml.name() == "nd") {
                    feature.nodeRefs.push_back(xml.attributes().value("ref").toLongLong());
                }
                else if(xml.name() == "tag") {
                    feature.tags[xml.attributes().value("k").toString()] = xml.attributes().value("v").toString();
                }
            }

            classifyFeature(map, std::move(feature));
        }
    }

    if(xml.hasError()) {
        LogWidget::addLog(QString("OSM parsing error: %1").arg(xml.errorString()), LogWidget::DANGER);
    }

    return map;
}
