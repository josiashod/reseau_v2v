#include "ui/windows/mainwindow.h"

#include <QDebug>
#include <QSqlDatabase>

#include <QApplication>
#include "utils/osmreader.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
//    Enregistrement des types pour le système de méta-objets de Qt
   qRegisterMetaType<QVector<Water>>("QVector<Water>");
   qRegisterMetaType<QVector<Park>>("QVector<Park>");
   qRegisterMetaType<QVector<Way>>("QMap<long long, Way>");
   qRegisterMetaType<QVector<Building>>("QVector<Building>");
    MainWindow w;
    w.show();
    return a.exec();
//    OsmReader::readOSMFile(":/lib/carte_5.osm");
    return 0;
}
