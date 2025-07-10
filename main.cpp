#include "ui/windows/mainwindow.h"
// #include "utils/osmreader.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Enregistrement des types pour le système de méta-objets de Qt
    // qRegisterMetaType<QVector<Park>>("QVector<Park>");
    // qRegisterMetaType<QVector<Way>>("QVector<Way>");
    // qRegisterMetaType<QVector<Building>>("QVector<Building>");


    // Démarrer la fenêtre principale
    MainWindow w;
    w.show();

   // OsmReader::readOSMFile("/home/josias/Documents/projects/reseau_v2v/lib/carte_1.osm");
   // qDebug() << "Affichage";
//    return 0;
    return a.exec();
}
