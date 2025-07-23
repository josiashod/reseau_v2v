#include "osmreader.h"
#include "dbmanager.h"
#include "../ui/widgets/logwidget.h"
#include <QSqlQuery>

OsmReader::OsmReader()
{}

void OsmReader::readOSMFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LogWidget::addLog("Unable to open OSM file", LogWidget::DANGER);
        return;
    }

    auto db = DBManager::instance().database();
    if (!db.transaction()) {
        LogWidget::addLog("Failed to start database transaction", LogWidget::DANGER);
        return;
    }

    QXmlStreamReader xml(&file);
    int nodeCount = 0, wayCount = 0, tagCount = 0;
    bool hasError = false;

    while (!xml.atEnd() && !xml.hasError() && !hasError) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            try {
                if (xml.name() == "node") {
                    readNode(xml, nodeCount, tagCount);
                } else if (xml.name() == "way") {
                    readWay(xml, wayCount, tagCount);
                } else if (xml.name() == "bounds") {
                    readBounds(xml);
                }
            } catch (const std::exception& e) {
                LogWidget::addLog(QString("Processing error: %1").arg(e.what()), LogWidget::DANGER);
                hasError = true;
            }
        }
    }

    if (hasError || xml.hasError()) {
        db.rollback();
        LogWidget::addLog(QString("Processing aborted: %1").arg(xml.errorString()), LogWidget::DANGER);
    } else {
        if (!db.commit()) {
            LogWidget::addLog("Commit failed", LogWidget::DANGER);
            db.rollback();
        } else {
            LogWidget::addLog(QString("Success: %1 nodes, %2 ways, %3 tags processed")
                                  .arg(nodeCount).arg(wayCount).arg(tagCount), LogWidget::SUCCESS);
        }
    }
    file.close();
}

void OsmReader::readBounds(QXmlStreamReader& xml) {
    QSqlQuery query(DBManager::instance().database());
    query.prepare("INSERT INTO bounds(minlat, minlon, maxlat, maxlon) VALUES(?,?,?,?)");
    query.addBindValue(xml.attributes().value("minlat").toDouble());
    query.addBindValue(xml.attributes().value("minlon").toDouble());
    query.addBindValue(xml.attributes().value("maxlat").toDouble());
    query.addBindValue(xml.attributes().value("maxlon").toDouble());

    if(!query.exec()) {
        LogWidget::addLog(QString("Bounds error: %1").arg(query.lastError().text()), LogWidget::DANGER);
    }
}

void OsmReader::readNode(QXmlStreamReader& xml, int& nodeCount, int& tagCount) {
    const long long id = xml.attributes().value("id").toString().toLongLong();
    QSqlQuery query(DBManager::instance().database());

    // Insert node
    query.prepare("INSERT INTO nodes(id, lat, lon) VALUES(?,?,?)");
    query.addBindValue(id);
    query.addBindValue(xml.attributes().value("lat").toDouble());
    query.addBindValue(xml.attributes().value("lon").toDouble());

    if(query.exec()) {
        nodeCount++;
    } else {
        LogWidget::addLog(QString("Node %1 error: %2").arg(id).arg(query.lastError().text()), LogWidget::DANGER);
        return;
    }

    // Process tags
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "node")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "tag") {
            query.prepare("INSERT INTO tags(element_type, element_id, t_key, t_value) VALUES(?,?,?,?)");
            query.addBindValue("node");
            query.addBindValue(id);
            query.addBindValue(xml.attributes().value("k").toString());
            query.addBindValue(xml.attributes().value("v").toString());

            if(query.exec()) {
                tagCount++;
            }
        }
    }
}

void OsmReader::readWay(QXmlStreamReader& xml, int& wayCount, int& tagCount) {
    const long long wayId = xml.attributes().value("id").toString().toLongLong();
    QSqlQuery query(DBManager::instance().database());

    // Insert way
    query.prepare("INSERT INTO ways(id) VALUES(?)");
    query.addBindValue(wayId);

    if(!query.exec()) {
        LogWidget::addLog(QString("Way %1 error: %2").arg(wayId).arg(query.lastError().text()), LogWidget::DANGER);
        return;
    }
    wayCount++;

    // Process nodes and tags
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "way")) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == "nd") {
                query.prepare("INSERT INTO way_node(node_id, way_id) VALUES(?,?)");
                query.addBindValue(xml.attributes().value("ref").toString().toLongLong());
                query.addBindValue(wayId);
                query.exec(); // We don't count these separately
            }
            else if (xml.name() == "tag") {
                query.prepare("INSERT INTO tags(element_type, element_id, t_key, t_value) VALUES(?,?,?,?)");
                query.addBindValue("way");
                query.addBindValue(wayId);
                query.addBindValue(xml.attributes().value("k").toString());
                query.addBindValue(xml.attributes().value("v").toString());

                if(query.exec()) {
                    tagCount++;
                }
            }
        }
    }
}
