#include "mainwindow.h"
#include <QCoreApplication>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QList>
#include <QString>

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTranslator translator;
//    const QStringList uiLanguages = QLocale::system().uiLanguages();
//    for (const QString &locale : uiLanguages) {
//        const QString baseName = "mulhouse_network_" + QLocale(locale).name();
//        if (translator.load(":/i18n/" + baseName)) {
//            a.installTranslator(&translator);
//            break;
//        }
//    }
    MainWindow w;
    w.show();
    return a.exec();
//    QCoreApplication a(argc, argv);

//        QFile file("/lib/mulhouse.osm");
//        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//            qWarning() << "Cannot open file:" << file.errorString();
//            return -1;
//        }

//        QXmlStreamReader xml(&file);
//        QList<OSMNode> nodes;
//        QList<OSMWay> roads;

//        while (!xml.atEnd() && !xml.hasError()) {
//            QXmlStreamReader::TokenType token = xml.readNext();

//            if (token == QXmlStreamReader::StartElement) {
//                if (xml.name() == "node") {
//                    // Récupérer les nœuds
//                    qint64 id = xml.attributes().value("id").toLongLong();
//                    double lat = xml.attributes().value("lat").toDouble();
//                    double lon = xml.attributes().value("lon").toDouble();
//                    nodes.append(OSMNode(id, lat, lon));
//                }
//                else if (xml.name() == "way") {
//                    // Récupérer les ways (routes)
//                    qint64 id = xml.attributes().value("id").toLongLong();
//                    OSMWay way(id);

//                    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "way")) {
//                        token = xml.readNext();
//                        if (token == QXmlStreamReader::StartElement && xml.name() == "nd") {
//                            qint64 nodeId = xml.attributes().value("ref").toLongLong();
//                            way.addNodeRef(nodeId);
//                        }
//                        else if (token == QXmlStreamReader::EndElement && xml.name() == "way") {
//                            // Vérifier les tags pour s'assurer que c'est une route
//                            xml.readNext();
//                            while (xml.tokenType() == QXmlStreamReader::StartElement) {
//                                if (xml.name() == "tag") {
//                                    QString key = xml.attributes().value("k").toString();
//                                    if (key == "highway") {
//                                        roads.append(way); // Ajouter seulement si c'est une route
//                                        break; // On sort après avoir ajouté
//                                    }
//                                }
//                                xml.readNext();
//                            }
//                        }
//                    }
//                }
//            }
//        }

//        // Gestion des erreurs
//        if (xml.hasError()) {
//            qWarning() << "XML Error:" << xml.errorString();
//            return -1;
//        }

//        file.close();

//        // Affichage des routes récupérées
//        for (const auto& road : roads) {
//            qDebug() << "Road ID:" << road.id << "Node References:" << road.nodeRefs;
//        }
}
