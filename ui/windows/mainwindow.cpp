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
#include <QtConcurrent>
#include "../dialog/addcardialog.h"
#include <QMessageBox>

// retourne le libelle à mettre dans le menu
QString menu_libelle(bool etat, const QString& libelle)
{
    return QString("%1 %2").arg((etat ? "Cacher" : "Afficher"), libelle);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    d_graph{},
    d_showRoads(true),
    d_showCarFreq(true),
    d_showBuildings(true),
    d_showParks(true),
    d_showMesh(false),
    d_showSidebar(true),
    d_isPlaying(false),
    d_selectedSpeed(3),
    d_elapsed_time(0)
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

    auto viewMenu = menuBar()->addMenu("Vue");
//    auto logMenu = menuBar()->addMenu("Logs");

    QAction *showRoadAct = new QAction{menu_libelle(d_showRoads, "les routes"), viewMenu};
    viewMenu->addAction(showRoadAct);

    QAction *showBuildindAct = new QAction{menu_libelle(d_showBuildings, "les immeubles"), viewMenu};
    viewMenu->addAction(showBuildindAct);

    QAction *showParcAct = new QAction{menu_libelle(d_showParks, "les parcs"), viewMenu};
    viewMenu->addAction(showParcAct);

    QAction *showFrequenceAct = new QAction{menu_libelle(d_showRoads, "les couvertures radio"), viewMenu};
    viewMenu->addAction(showFrequenceAct);

    QAction *showHexAct = new QAction{menu_libelle(d_showMesh, "le decoupage territorial"), viewMenu};
    viewMenu->addAction(showHexAct);

    QAction *showSidebarAct = new QAction{menu_libelle(d_showSidebar, "la menu latérale"), viewMenu};
    viewMenu->addAction(showSidebarAct);

    // creation du main widget pour l'application
    QWidget* mainWidget = new QWidget{this};
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWidget);

    mainWidget->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(mainWidget);

    d_logsView = new LogWidget{this};
    d_mapView = new MapWidget{this, &d_graph};

    mainLayout->addWidget(d_mapView, 1);
    // creation des differents layout
    d_rightSidebar = new QWidget();
    QVBoxLayout *rightSidebarLayout = new QVBoxLayout(d_rightSidebar);
    auto *simulationControlLayout = new QHBoxLayout(d_rightSidebar);

    mainLayout->addWidget(d_rightSidebar);
    rightSidebarLayout->addWidget(d_logsView, 1);
    d_addCarButton = new QPushButton{"Ajouter des voitures", this};
    d_timeLabel = new QLabel{"Temps écoulé: "+(QTime(0, 0, 0).addSecs(d_elapsed_time)).toString("hh:mm:ss"), this};
    rightSidebarLayout->addWidget(d_timeLabel, 0, Qt::AlignCenter);
    rightSidebarLayout->addWidget(d_addCarButton);
    d_addCarButton->setEnabled(false);

    auto clearLogButton = new QPushButton{"Effacer les logs", this};
    rightSidebarLayout->addWidget(clearLogButton);
    rightSidebarLayout->addLayout(simulationControlLayout);
    d_playButton = new QPushButton{"Lancer la simulation", this};
    d_speedSelector = new QComboBox{this};

    d_playButton->setEnabled(false);
    d_speedSelector->setEnabled(false);

    simulationControlLayout->addWidget(d_playButton);
    simulationControlLayout->addWidget(d_speedSelector);

    d_rightSidebar->setContentsMargins(0, 0, 0, 0);
    mainLayout->setContentsMargins(0, 0, 0, 0);


    // actions connects
    connect(showRoadAct, &QAction::triggered, this, &MainWindow::onShowHideRoads);
    connect(showBuildindAct, &QAction::triggered, this, &MainWindow::onShowHideBuildings);
    connect(showParcAct, &QAction::triggered, this, &MainWindow::onShowHideParks);
    connect(showSidebarAct, &QAction::triggered, this, &MainWindow::onShowHideSidebar);
    connect(showFrequenceAct, &QAction::triggered, this, &MainWindow::onShowHideFreq);
    connect(showHexAct, &QAction::triggered, this, &MainWindow::onShowHideMesh);

    // buttons control for the simulation connects
    connect(d_playButton, &QPushButton::clicked, this, &MainWindow::onPlay);
    connect(d_addCarButton, &QPushButton::clicked, this, &MainWindow::onAddCar);
    connect(clearLogButton, &QPushButton::clicked, this, &MainWindow::onClearLog);

    createVComboBox();
    selectSpeed();
    connect(d_speedSelector, &QComboBox::currentIndexChanged, this, &MainWindow::updateSpeedSelector);

    //connections aux évènements de la map
    connect(d_mapView, &MapWidget::isLoading, this, &MainWindow::onMapLoading);
    connect(d_mapView, &MapWidget::isLoaded, this, &MainWindow::onMapLoaded);
//    connect(d_mapView, &MapWidget::addElementToPartialSelection, this, &MainWindow::onAddCarToPartialSelection);
//    connect(d_mapView, &MapWidget::removeElementFromPartialSelection, this, &MainWindow::onRemoveCarFromPartialSelection);
//    connect(d_mapView, &MapWidget::deletePartialSelection, this, &MainWindow::onDeletePartialSelection);
//    connect(d_mapView, &MapWidget::persistPartialSelection, this, &MainWindow::onPersistSelection);

    d_timer = new QTimer(this);
    // Mettre l'interval de rafraichissement de l'affichage
    d_timer->setInterval(1000 / FPS);
    // ecoute chaque fois que le timer signal etre arrivé à la fin d'un interval
    connect(d_timer, &QTimer::timeout, this, &MainWindow::updateFrame);

    updatePlayButtonLabel();
}

void MainWindow::onShowHideBuildings(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showBuildings = !d_showBuildings;
    action->setText(menu_libelle(d_showBuildings, "les immeubles"));
    d_mapView->setShowBuilding(d_showBuildings);
}

void MainWindow::onShowHideRoads(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showRoads = !d_showRoads;
    d_mapView->setShowRoad(d_showRoads);
    action->setText(menu_libelle(d_showRoads, "les routes"));
}

void MainWindow::onShowHideFreq(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showCarFreq = !d_showCarFreq;
    emit freqVisibilityChanged(d_showCarFreq);
    action->setText(menu_libelle(d_showCarFreq, "les couvertures radio"));
}

void MainWindow::onShowHideParks(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showParks = !d_showParks;
    d_mapView->setShowPark(d_showParks);
    action->setText(menu_libelle(d_showParks, "les parcs"));
}

void MainWindow::onShowHideMesh(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showMesh = !d_showMesh;
    d_mapView->setShowHex(d_showMesh);
    action->setText(menu_libelle(d_showMesh, "le decoupage territorial"));
}

void MainWindow::onShowHideSidebar(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showSidebar = !d_showSidebar;

    if(d_showSidebar)
        d_rightSidebar->show();
    else
        d_rightSidebar->hide();

    action->setText(menu_libelle(d_showSidebar, "la menu latérale"));
}

void MainWindow::onMapLoading(bool)
{
    d_logsView->addLog("Chargement de la carte...............", LogWidget::WARNING);
}

void MainWindow::onMapLoaded(bool)
{
    d_addCarButton->setEnabled(true);

    d_logsView->addLog("La carte a été chargée !!!", LogWidget::SUCCESS);
}

void  MainWindow::updatePlayButtonLabel()
{
    if(d_isPlaying)
        d_playButton->setText("Mettre en pause");
    else
        d_playButton->setText("Lancer la simulation");

    playOrPause();
}

void MainWindow::onPlay(bool)
{
    d_isPlaying = !d_isPlaying;

    updatePlayButtonLabel();
}

void MainWindow::onClearLog()
{
    d_logsView->clearLog();
}

void MainWindow::createVComboBox()
{
    d_speedSelector->addItems(d_listOfSpeeds);
}

void MainWindow::updateSpeedSelector(int i)
{
    d_selectedSpeed = i;
    selectSpeed();
//    accelerate();
    playOrPause();
}

//void MainWindow::onCarReachEndRoad(long long currentPos)
//{
//    Car* car = qobject_cast<Car *>(sender());
//    osm::Node* newnode;
//    do
//    {
//        newnode = d_graph.getRandomNode();
//    }
//    while(!newnode || newnode->id() == currentPos);

//    auto path = d_graph.dijkstraPath(currentPos, newnode->id());
//    car->updatePath(path);
//}

void MainWindow::playOrPause()
{
    if(d_isPlaying)
        d_timer->start(1000 / FPS);
    else
        d_timer->stop();
}

void MainWindow::accelerate()
{
    // for(size_t i = 0; i < d_cars.size(); i++)
    // {
    //     d_cars[i].get()->accelerate(d_speeds[d_selectedSpeed]);
    // }
}

void MainWindow::updateFrame()
{
    double interval = 1000 / FPS;
    d_elapsed_time += interval * d_speeds[d_selectedSpeed];
    QString time = (QTime(0, 0, 0).addSecs(d_elapsed_time / 1000)).toString("hh:mm:ss");
    d_timeLabel->setText("Temps écoulé: " + time);

    emit timeout(interval * d_speeds[d_selectedSpeed]);
}

void MainWindow::selectSpeed()
{
    d_speedSelector->setCurrentIndex(d_selectedSpeed);
}

void MainWindow::onAddCar()
{
    d_isPlaying = false;
    updatePlayButtonLabel();
    playOrPause();

    auto dialog = new AddCarDialog(this);
    dialog->show();

    connect(dialog, &AddCarDialog::create_car, this, &MainWindow::addCar);
}

void MainWindow::addCar(int nb, double speed, double freq, double intensity)
{
    for(int i = 0; i < nb; i++)
    {
        std::vector<osm::Node*> path(0);
        osm::Node* n1 = nullptr;

        n1 = d_graph.getRandomNode();
        path.push_back(n1);

        d_cars.push_back(std::make_unique<Car>(path, speed, freq, intensity));
        auto* car = d_cars.back().get();

        connect(this, &MainWindow::timeout, car, &Car::move);
        connect(this, &MainWindow::freqVisibilityChanged, car, &Car::updateFrequenceVisibility);
        connect(car, &Car::hasReachEndOfPath, this, &MainWindow::onCarHasReachEndOfPath);

        d_mapView->addCar(car);
        car->updateFrequenceVisibility(d_showCarFreq);
    }

    // on active les boutons de play et vitesse
    d_playButton->setEnabled(true);
    d_speedSelector->setEnabled(true);
}

void MainWindow::onCarHasReachEndOfPath()
{
    auto car = qobject_cast<Car*>(sender());

    std::vector<osm::Node*> path(0);
    auto* newnode = d_graph.getRandomNode();
    if(newnode)
        path = d_graph.dijkstraPath(car->to()->id(), newnode->id());

    if(!path.empty() && path.size() > 2)
    {
        QMetaObject::invokeMethod(this, [c = car, path]() {
           c->setPath(path);
        }, Qt::QueuedConnection);
    }
}

//void MainWindow::onAddCarToPartialSelection(int id)
//{
//    d_car_partially_selected.push_back(id);
//    auto it = std::find_if( d_cars.begin(), d_cars.end(),
//        [id](const auto& car) {
//            return *car == id;
//        });

//    if (it != d_cars.end()) {
//        auto car = (*it).get();
//        car->partiallySelected();
//    }
//}

//void MainWindow::onRemoveCarFromPartialSelection(int id)
//{
//    // on supprime l'id de la voiture des voitures partiellement selectionné
//    auto i = std::find(d_car_partially_selected.begin(), d_car_partially_selected.end(), id);
//    if (i != d_car_partially_selected.end())
//    {
//        d_car_partially_selected.erase(i);
//    }

//    // on la selection de la voiture
//    auto it = std::find_if(d_cars.begin(), d_cars.end(),
//        [id](const auto& car) {
//            return *car == id;
//        });

//    if (it != d_cars.end()) {
//        auto car = (*it).get();
//        car->removeMark();
//    }
//}

//void MainWindow::onPersistSelection()
//{
//    for(const int& id: d_car_partially_selected)
//    {
//        auto it = std::find_if( d_cars.begin(), d_cars.end(),
//            [id](const auto& car) {
//                return *car == id;
//            });

//        if (it != d_cars.end())
//        {
//            auto car = (*it).get();
//            car->selected();
//        }

//        d_car_selection.push_back(id);
//    }

//    std::sort(d_car_selection.begin(), d_car_selection.end());
//    d_car_partially_selected.clear();
//}

//void MainWindow::onDeletePartialSelection()
//{
//    for(const int& id: d_car_partially_selected)
//    {
//        auto it = std::find_if( d_cars.begin(), d_cars.end(),
//            [id](const auto& car) {
//                return *car == id;
//            });

//        if (it != d_cars.end()) {
//            auto car = (*it).get();
//            car->removeMark();
//        }
//    }
//    d_car_partially_selected.clear();
//}

MainWindow::~MainWindow()
{
    delete d_mapView;
    delete d_logsView;
    delete d_rightSidebar;
    delete d_timer;
    for(auto* child: menuBar()->children())
        delete child;
}
