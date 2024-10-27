#include "mainwindow.h"
#include <QScreen>
#include <QApplication>
#include <QLayout>
#include <QWidget>
#include <QLabel>
#include <QMenuBar>

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
    //setMinimumSize(width - (width * 0.40), height - (height * 0.20));
    setFixedSize(width , height);

    auto viewMenu = menuBar()->addMenu("Vue");
    auto logMenu = menuBar()->addMenu("Log");

    QAction *showRoadAct = new QAction{"Afficher/Masquer les routes"};
    viewMenu->addAction(showRoadAct);

    QAction *showBuildindAct = new QAction{"Afficher/Masquer les immeubles"};
    viewMenu->addAction(showBuildindAct);

    QAction *showWaterAct = new QAction{"Afficher/Masquer les eaux"};
    viewMenu->addAction(showWaterAct);

    QAction *showParcAct = new QAction{"Afficher/Masquer les espaces verts"};
    viewMenu->addAction(showParcAct);

    QAction *showDescAct = new QAction{"Afficher/Masquer les description"};
    viewMenu->addAction(showDescAct);

    connect(showRoadAct, &QAction::triggered, this, &MainWindow::onShowHideRoads);
    connect(showBuildindAct, &QAction::triggered, this, &MainWindow::onShowHideBuildings);
    connect(showWaterAct, &QAction::triggered, this, &MainWindow::onShowHideWaters);
    connect(showParcAct, &QAction::triggered, this, &MainWindow::onShowHideParks);
    connect(showDescAct, &QAction::triggered, this, &MainWindow::onShowHideDescs);

    auto mainWidget {new QWidget{this}};
    auto *mainHlayout = new QHBoxLayout();
    mainHlayout->setContentsMargins(0, 0, 0, 0);
    mainWidget->setLayout(mainHlayout);
    setCentralWidget(mainWidget);

    d_map = new MapViewer{this};
    mainHlayout->addWidget(d_map, 1);
    mainHlayout->addWidget(new QLabel{"test"});
}

void MainWindow::onShowHideBuildings(bool)
{
    d_showBuildings = !d_showBuildings;
    d_map->setShowBuilding(d_showBuildings);
}

void MainWindow::onShowHideRoads(bool)
{
    d_showRoads = !d_showRoads;
    d_map->setShowRoad(d_showRoads);
}

void MainWindow::onShowHideWaters(bool)
{
    d_showWaters = !d_showWaters;
    d_map->setShowWater(d_showWaters);
}

void MainWindow::onShowHideParks(bool)
{
    d_showParks = !d_showParks;
    d_map->setShowPark(d_showParks);
}

void MainWindow::onShowHideDescs(bool)
{
    d_showDescs = !d_showDescs;
    d_map->setShowDescription(d_showDescs);
}

MainWindow::~MainWindow()
{
}
