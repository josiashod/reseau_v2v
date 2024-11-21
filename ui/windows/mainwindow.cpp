#include "mainwindow.h"
#include <QScreen>
#include <QApplication>
#include <QLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMenuBar>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), d_graph{}
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
//    setMinimumSize(width - (width * 0.15), height - (height * 0.15));
    setFixedSize(width - (width * 0.01), height - (height * 0.10));

//    this->setStyleSheet("QPushButton { background-color: #CDCDCD; border: 1px solid black; padding: 4px 3px; cursor: pointer; }");

    auto viewMenu = menuBar()->addMenu("Vue");
//    auto logMenu = menuBar()->addMenu("Logs");

    QAction *showRoadAct = new QAction{"Show/hide roads", viewMenu};
    viewMenu->addAction(showRoadAct);

    QAction *showBuildindAct = new QAction{"Show/hide buildings", viewMenu};
    viewMenu->addAction(showBuildindAct);

    QAction *showParcAct = new QAction{"Show/hide green spaces", viewMenu};
    viewMenu->addAction(showParcAct);

    QAction *showFrequenceAct = new QAction{"Show/hide car frequences", viewMenu};
    viewMenu->addAction(showFrequenceAct);

    QAction *showLogsAct = new QAction{"Show/hide sidebar", viewMenu};
    viewMenu->addAction(showLogsAct);

    // creation du main widget pour l'application
    auto mainWidget {new QWidget{this}};
    auto *mainLayout = new QHBoxLayout();

//    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainWidget->setLayout(mainLayout);
    mainWidget->setContentsMargins(10, 10, 10, 10);
    setCentralWidget(mainWidget);

    // creation des differents layout
    d_rightSidebar = new QVBoxLayout{this};
//    d_rightSidebar->setContentsMargins(0, 0, 10, 0);
    auto *simulationControlLayout = new QHBoxLayout{this};

    d_logsView = new LogWidget{this};
    d_mapView = new MapWidget{this, &d_graph};

    mainLayout->addWidget(d_mapView, 1);
    d_rightSidebar->addWidget(d_logsView, 1);
    d_rightSidebar->addWidget(new QPushButton{"Add cars"});
    d_rightSidebar->addWidget(new QPushButton{"Clear logs"});
    d_rightSidebar->addLayout(simulationControlLayout);
    d_playButton = new QPushButton{"play"};
    d_speedSelector = new QComboBox{};
//    simulationControlLayout->addSpacing(100);


    simulationControlLayout->addWidget(d_playButton);
    simulationControlLayout->addWidget(d_speedSelector);
//    simulationControlLayout->setContentsMargins(0,0,1000,0);
//    simulationControlLayout->addWidget(new QPushButton{"test"}, 0, Qt::AlignHCenter);
//    vLayout->addLayout(simulationControlLayout);
//    mainLayout->addLayout(vLayout, 1);

    d_rightSidebar->setEnabled(false);
//    d_logsView->hide();
    d_showLogs = false;
    mainLayout->addLayout(d_rightSidebar);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // actions connects
    connect(showRoadAct, &QAction::triggered, this, &MainWindow::onShowHideRoads);
    connect(showBuildindAct, &QAction::triggered, this, &MainWindow::onShowHideBuildings);
    connect(showParcAct, &QAction::triggered, this, &MainWindow::onShowHideParks);
    connect(showLogsAct, &QAction::triggered, this, &MainWindow::onShowHideLogs);
//    connect(showFrequenceAct, &QAction::triggered, this, &MainWindow::onShowHideFreq);

    // buttons control for the simulation connects
    connect(d_playButton, &QPushButton::clicked, this, &MainWindow::onPlay);

    createVComboBox();
    selectSpeed();
    connect(d_speedSelector, &QComboBox::currentIndexChanged, this, &MainWindow::updateSpeedSelector);

    //map connects
    connect(d_mapView, &MapWidget::isLoading, this, &MainWindow::onMapLoading);
    connect(d_mapView, &MapWidget::isLoaded, this, &MainWindow::onMapLoaded);

    d_timer = new QTimer(this);
    // Mettre l'interval de rafraichissement de l'affichage
    d_timer->setInterval(1000 / FPS);
    // ecoute chaque fois que le timer signal etre arrivé à la fin d'un interval
    connect(d_timer, &QTimer::timeout, this, &MainWindow::updateCarsPosition);

    updatePlayButtonLabel();
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

/*void MainWindow::onShowHideWaters(bool)
{
    d_showWaters = !d_showWaters;
    d_mapView->setShowWater(d_showWaters);
}*/

void MainWindow::onShowHideParks(bool)
{
    d_showParks = !d_showParks;
    d_mapView->setShowPark(d_showParks);
}

void MainWindow::onShowHideLogs(bool)
{
    d_showLogs = !d_showLogs;

//    if(d_showLogs)
        d_rightSidebar->setEnabled(d_showLogs);
//    else
//        d_rightSidebar->hide();
        qDebug() << "cache" << d_rightSidebar->isEnabled();
}

void MainWindow::onMapLoading(bool)
{
    d_logsView->addLog("The map is loading...............", LogWidget::WARNING);
}

void MainWindow::onMapLoaded(bool)
{
    d_logsView->addLog("The map has been loaded", LogWidget::SUCCESS);

    for(int i = 0; i < 1; i++)
    {
        osm::Node*  n = d_graph.getRandomNode();

        if(n)
        {
            d_cars.push_back(std::make_unique<Car>(Car{n}));
            d_mapView->addCarSymbols(d_cars.back()->pixmap(), d_cars.back()->ellipse());
        }
    }
}

void  MainWindow::updatePlayButtonLabel()
{
    if(d_isPlaying)
        d_playButton->setText("Pause");
    else
        d_playButton->setText("Play");

    playOrPause();
}

void MainWindow::onPlay(bool)
{
    d_isPlaying = !d_isPlaying;

    updatePlayButtonLabel();
}

void MainWindow::createVComboBox()
{
    d_speedSelector->addItems(d_listOfSpeeds);
}

void MainWindow::updateSpeedSelector(int i)
{
    d_selectedSpeed = i;
    selectSpeed();
    accelerate();
}

void MainWindow::playOrPause()
{
    if(d_isPlaying)
        d_timer->start();
    else
        d_timer->stop();
}

void MainWindow::accelerate()
{
    for(size_t i = 0; i < d_cars.size(); i++)
    {
        d_cars[i].get()->accelerate(d_speeds[d_selectedSpeed]);
    }
}

void MainWindow::updateCarsPosition()
{
    double interval = 1000 / FPS;

    for(size_t i = 0; i < d_cars.size(); i++)
    {
        d_cars[i].get()->update(interval);
    }
}

void MainWindow::selectSpeed()
{
    d_speedSelector->setCurrentIndex(d_selectedSpeed);
}

MainWindow::~MainWindow()
{
    delete d_mapView;
    delete d_logsView;
}
