#include "ui/windows/mainwindow.h"
// #include "utils/osmreader.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Démarrer la fenêtre principale
    MainWindow w;
    w.show();

   // OsmReader::readOSMFile("/home/josias/Téléchargements/map.osm");
   // qDebug() << "Affichage";
//    return 0;
    return a.exec();
}
