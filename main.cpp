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
#include "osmreader.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();

}
