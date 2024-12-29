#include "ui/windows/mainwindow.h"
#include "utils/osmreader.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Enregistrement des types pour le système de méta-objets de Qt
//    qRegisterMetaType<QVector<Water>>("QVector<Water>");
    qRegisterMetaType<QVector<Park>>("QVector<Park>");
    qRegisterMetaType<QMap<long long, Way>>("QMap<long long, Way>");
    qRegisterMetaType<QVector<Building>>("QVector<Building>");


//     Démarrer la fenêtre principale
    MainWindow w;
    w.show();

    return a.exec();
//    OsmReader::readOSMFile("/home/josh/Téléchargements/data");
//    qDebug() << "Affichage";
//    return 0;
}
