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
//    auto logMenu = menuBar()->addMenu("Logs");

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

    QAction *showLogsAct = new QAction{"Afficher/Masquer les logs"};
    viewMenu->addAction(showLogsAct);

    connect(showRoadAct, &QAction::triggered, this, &MainWindow::onShowHideRoads);
    connect(showBuildindAct, &QAction::triggered, this, &MainWindow::onShowHideBuildings);
    connect(showWaterAct, &QAction::triggered, this, &MainWindow::onShowHideWaters);
    connect(showParcAct, &QAction::triggered, this, &MainWindow::onShowHideParks);
    connect(showDescAct, &QAction::triggered, this, &MainWindow::onShowHideDescs);
    connect(showLogsAct, &QAction::triggered, this, &MainWindow::onShowHideLogs);

    auto mainWidget {new QWidget{this}};
    auto *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);

    d_mapView = new MapWidget{this};
    mainLayout->addWidget(d_mapView, 1);

    d_logsView = new LogWidget{this};
    d_logsView->hide();
    mainLayout->addWidget(d_logsView);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::onShowHideBuildings(bool)
{
    d_showBuildings = !d_showBuildings;
    d_mapView->setShowBuilding(d_showBuildings);
}

void MainWindow::onShowHideRoads(bool)
{
    d_showRoads = !d_showRoads;
    d_mapView->setShowRoad(d_showRoads);
}

void MainWindow::onShowHideWaters(bool)
{
    d_showWaters = !d_showWaters;
    d_mapView->setShowWater(d_showWaters);
}

void MainWindow::onShowHideParks(bool)
{
    d_showParks = !d_showParks;
    d_mapView->setShowPark(d_showParks);
}

void MainWindow::onShowHideDescs(bool)
{
    d_showDescs = !d_showDescs;
    d_mapView->setShowDescription(d_showDescs);
}

void MainWindow::onShowHideLogs(bool)
{
    d_showLogs = !d_showLogs;

    if(d_showLogs)
        d_logsView->show();
    else
        d_logsView->hide();
}

MainWindow::~MainWindow()
{
}
