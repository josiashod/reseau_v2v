#include "mainwindow.h"
#include <QScreen>
#include <QApplication>
#include <QLayout>
#include <QWidget>

#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    creerInterface();
}

void MainWindow::creerInterface()
{
    setWindowTitle("SIMULATION V2V MULHOUSE");
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();
    setMinimumSize(width - (width * 0.40), height - (height * 0.20));

    auto mainWidget {new QWidget{this}};
    auto *mainHlayout = new QHBoxLayout();
    mainWidget->setLayout(mainHlayout);
    setCentralWidget(mainWidget);

    d_map = new MapViewer{this};
    mainHlayout->addWidget(d_map);

    QFile file(":/lib/mulhouse.osm");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file:" << file.errorString();
        return ;
    }

    file.close();
}

MainWindow::~MainWindow()
{
}
