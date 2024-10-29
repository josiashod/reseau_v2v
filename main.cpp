#include "ui/windows/mainwindow.h"

#include <QDebug>
#include <QSqlDatabase>

#include <QApplication>
#include "utils/osmreader.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
//    OsmReader::readOSMFile("/home/josh/Téléchargements/osm/data1");
    return 0;
}
