#include "osmreader.h"
#include "dbmanager.h"

#include <QSqlQuery>
#include <vector>

OsmReader::OsmReader()
{}

void OsmReader::readOSMFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open OSM file.";
        return;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            if (xml.name().toString() == "node") {
                readNode(xml);
            } else if (xml.name().toString() == "way") {
                readWay(xml);
            }
            else if (xml.name().toString() == "bounds") {
                readBounds(xml);  // Pour des données plus complexes (comme les bâtiments)
            }/*else if (xml.name().toString() == "relation") {
                readRelation(xml);  // Pour des données plus complexes (comme les bâtiments)
            }*/
        }
    }

    if (xml.hasError()) {
        qDebug() << "Error while reading file:" << xml.errorString();
    }

    file.close();
}

void OsmReader::readBounds(QXmlStreamReader& xml) {
    double minlat = xml.attributes().value("minlat").toDouble();
    double minlon = xml.attributes().value("minlon").toDouble();
    double maxlat = xml.attributes().value("maxlat").toDouble();
    double maxlon = xml.attributes().value("maxlon").toDouble();

    auto db = DBManager::instance().database();
    QSqlQuery query(db);
    query.prepare("INSERT INTO bounds(minlat, minlon, maxlat, maxlon) VALUES(:minlat, :minlon, :maxlat, :maxlon)");
    query.bindValue(":minlat", minlat);
    query.bindValue(":minlon", minlon);
    query.bindValue(":maxlat", maxlat);
    query.bindValue(":maxlon", maxlon);
    if(query.exec())
    {
       qDebug() << "add bounds success";
    }
    else
    {
        qDebug() << "add bounds error " << db.lastError().text();;
    }

    query.finish();
    query.clear();
}

void OsmReader::readNode(QXmlStreamReader& xml) {
    double lat = xml.attributes().value("lat").toDouble();
    double lon = xml.attributes().value("lon").toDouble();
    long long id = xml.attributes().value("id").toString().toLongLong();

    auto db = DBManager::instance().database();
    QSqlQuery query(db);
    query.prepare("INSERT INTO nodes(id, lat, lon) VALUES(:id, :lat, :lon)");
    query.bindValue(":id", id);
    query.bindValue(":lat", lat);
    query.bindValue(":lon", lon);
    if(query.exec())
    {
       qDebug() << "add node success";
    }
    else
    {
        qDebug() << "add node error " << db.lastError().text();;
    }

    // Parcourir les nœuds du chemin
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name().toString() == "node"))
    {
        xml.readNext();
        // Gérer les tags pour savoir si c'est une route ou un bâtiment
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "tag")
        {
            query.prepare("INSERT INTO tags(element_type, element_id, t_key, t_value) VALUES(:element_type, :element_id, :key, :value)");
            query.bindValue(":element_type", "node");
            query.bindValue(":element_id", id);
            query.bindValue(":key", xml.attributes().value("k").toString());
            query.bindValue(":value", xml.attributes().value("v").toString());
            if(query.exec())
               {
                   qDebug() << "add node tag";
               }
            else
                qDebug() << "Erreur";
        }
    }

    query.finish();
    query.clear();
}


void OsmReader::readWay(QXmlStreamReader& xml)
{
    long long wayId = xml.attributes().value("id").toString().toLongLong();
    std::vector<long long> node_ids;
    auto db = DBManager::instance().database();
    QSqlQuery query(db);

    // Parcourir les nœuds du chemin
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name().toString() == "way"))
    {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "nd")
        {
            node_ids.push_back(xml.attributes().value("ref").toString().toLongLong());
        }

        // Gérer les tags pour savoir si c'est une route ou un bâtiment
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name().toString() == "tag")
        {
            query.prepare("INSERT INTO tags(element_type, element_id, t_key, t_value) VALUES(:element_type, :element_id, :key, :value)");
            query.bindValue(":element_type", "way");
            query.bindValue(":element_id", wayId);
            query.bindValue(":key", xml.attributes().value("k").toString());
            query.bindValue(":value", xml.attributes().value("v").toString());
            if(query.exec())
               {
                   qDebug() << "node add tag succes";
               }
        }
    }

    query.prepare("INSERT INTO ways(id) VALUES(:id)");
    query.bindValue(":id", wayId);
    if(query.exec())
   {
       qDebug() << "add way success";
   }

    for(auto id: node_ids)
    {
        query.prepare("INSERT INTO way_node(node_id, way_id) VALUES(:node_id, :way_id)");
        query.bindValue(":way_id", wayId);
        query.bindValue(":node_id", id);
        if(query.exec())
           {
               qDebug() << "add node way success";
           }
    }

    query.finish();
    query.clear();
}

void OsmReader::readRelation(QXmlStreamReader& xml) {
   QString relationId = xml.attributes().value("id").toString();
   qDebug() << "Relation ID:" << relationId;

   // Parcourir les membres de la relation
   while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "relation")) {
       xml.readNext();
       if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "member") {
           QString type = xml.attributes().value("type").toString();
           QString ref = xml.attributes().value("ref").toString();
           qDebug() << "  Member type:" << type << "Reference:" << ref;
       }

       // Gérer les tags pour identifier le type de relation (par exemple, pour les bâtiments)
       if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "tag") {
           QString key = xml.attributes().value("k").toString();
           QString value = xml.attributes().value("v").toString();
           qDebug() << "  Tag:" << key << "Value:" << value;
       }
   }
}
