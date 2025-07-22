#include "mainwindow.h"
#include <QScreen>
#include <QApplication>
#include <QLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMenuBar>
#include <QMessageBox>
#include <QTime>

#include "ui/widgets/logwidget.h"
#include "../../core/car.h"
#include "../dialog/addcardialog.h"

const int FPS = 60;

// retourne le libelle à mettre dans le menu
QString menu_libelle(bool etat, const QString& libelle)
{
    return QString("%1 %2").arg((etat ? "Cacher" : "Afficher"), libelle);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    d_timer{this},
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

    setMinimumSize(width - (width * 0.3), height - (height * 0.15));
    // setFixedSize(width - (width * 0.01), height - (height * 0.10));

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

    mainLayout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(mainWidget);

    LogWidget::init(this);
    auto logsWidget = LogWidget::instance();
    logsWidget->setFixedWidth(350);
    d_mapWidget = new MapWidget{this, &d_graph};

    d_mapWidget->setStyleSheet("border: 3px solide blue");

    mainLayout->addWidget(d_mapWidget, 1);
    // creation des differents layout
    d_rightSidebar = new QWidget();
    QVBoxLayout *rightSidebarLayout = new QVBoxLayout(d_rightSidebar);
    auto *simulationControlLayout = new QHBoxLayout(d_rightSidebar);

    mainLayout->addWidget(d_rightSidebar);
    rightSidebarLayout->addWidget(logsWidget, 1);
    d_addCarButton = new QPushButton{"Ajouter des voitures", this};
    d_addCarButton->setShortcut(Qt::CTRL|Qt::SHIFT|Qt::Key_A);
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
    connect(d_mapWidget, &MapWidget::isLoaded, this, &MainWindow::onMapLoaded);
    connect(this, &MainWindow::timeout, d_mapWidget, &MapWidget::checkCarsConnections);

    updatePlayButton();
}

void MainWindow::onShowHideBuildings(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showBuildings = !d_showBuildings;
    action->setText(menu_libelle(d_showBuildings, "les immeubles"));
    d_mapWidget->setShowBuilding(d_showBuildings);
}

void MainWindow::onShowHideRoads(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showRoads = !d_showRoads;
    d_mapWidget->setShowRoad(d_showRoads);
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
    d_mapWidget->setShowPark(d_showParks);
    action->setText(menu_libelle(d_showParks, "les parcs"));
}

void MainWindow::onShowHideMesh(bool)
{
    QAction* action = qobject_cast<QAction*>(sender());
    d_showMesh = !d_showMesh;
    d_mapWidget->setShowHex(d_showMesh);
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

void MainWindow::onMapLoaded(bool loaded)
{
    if(loaded)
    {
        d_addCarButton->setEnabled(true);
        LogWidget::addLog("La carte a été chargée !!!", LogWidget::SUCCESS);
    }
    else
    {
        LogWidget::addLog("Chargement de la carte...............", LogWidget::WARNING);
    }
}

void  MainWindow::updatePlayButton()
{
    if(d_isPlaying)
        d_playButton->setText("Mettre en pause");
    else
        d_playButton->setText("Lancer la simulation");

    playOrPause();
}

void MainWindow::onPlay()
{
    d_isPlaying = !d_isPlaying;

    updatePlayButton();
}

void MainWindow::onClearLog()
{
    LogWidget::clear();
}

void MainWindow::createVComboBox()
{
    QStringList listOfSpeeds;

    for (const auto& speed: d_speeds)
    {
        listOfSpeeds.append(QString::number(speed));
    }
    listOfSpeeds[3] = "Normale";

    d_speedSelector->addItems(listOfSpeeds);
}

void MainWindow::updateSpeedSelector(int i)
{
    d_selectedSpeed = i;
    selectSpeed();
    emit accelarate(d_speeds[i]);
}

void MainWindow::playOrPause()
{
    if(d_timer.interval() == 0)
    {
        // Mettre l'interval de rafraichissement de l'affichage
        d_timer.setInterval(1000 / FPS);
        // ecoute chaque fois que le timer signal etre arrivé à la fin d'un interval
        connect(&d_timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    }

    if(d_isPlaying)
        d_timer.start(1000 / FPS);
    else
        d_timer.stop();
}

void MainWindow::updateFrame()
{
    double interval = 1000 / FPS;
    d_elapsed_time += interval * d_speeds[d_selectedSpeed];
    QString time = (QTime(0, 0, 0).addSecs(d_elapsed_time / 1000)).toString("hh:mm:ss");
    d_timeLabel->setText("Temps écoulé: " + time);

    emit timeout(interval);
}

void MainWindow::selectSpeed()
{
    d_speedSelector->setCurrentIndex(d_selectedSpeed);
}

void MainWindow::onAddCar()
{
    d_isPlaying = false;
    updatePlayButton();
    playOrPause();

    auto dialog = new AddCarDialog(this);
    dialog->show();

    connect(dialog, &AddCarDialog::create_car, this, &MainWindow::addCar);
}

void MainWindow::onSimulationPauseRequest(bool pause)
{
    if(pause)
    {
        d_isPlaying = false;
        updatePlayButton();
    }
    else
    {
        d_isPlaying = true;
        updatePlayButton();
    }

}

void MainWindow::addCar(int nb, double speed, double freq, double intensity)
{
    for(int i = 0; i < nb; i++)
    {
        std::vector<osm::Node*> path(0);
        osm::Node* n1 = nullptr;

        n1 = d_graph.getRandomNode();
        path.push_back(n1);

        // d_cars.push_back(std::make_unique<Car>(path, speed, freq, intensity));
        // auto* car = d_cars.back().get();
        auto* car = new Car(path, speed, freq, intensity);

        connect(this, &MainWindow::timeout, car, &Car::move);
        connect(this, &MainWindow::accelarate, car, &Car::accelerate);
        connect(this, &MainWindow::freqVisibilityChanged, car, &Car::updateFrequenceVisibility);
        connect(car, &Car::hasReachEndOfPath, this, &MainWindow::onCarHasReachEndOfPath);
        connect(car, &Car::requestSimulationPause, this, &MainWindow::onSimulationPauseRequest);
        connect(car, &Car::isConnectedToCars, this, [logs = LogWidget::instance(), this](){
            if(auto* car = qobject_cast<Car*>(sender()))
            {
                QTimer::singleShot(1000, [car, logs, time = &d_elapsed_time](){
                    logs->addLog(QString("================ %1  ================").arg((QTime(0, 0, 0).addSecs(*time / 1000)).toString("hh:mm:ss")),
                                 LogWidget::INFO);
                    logs->addLog(car->toString(), LogWidget::SUCCESS);
                });
            }
        });

        d_mapWidget->addCar(car);
        car->updateFrequenceVisibility(d_showCarFreq);
    }

    // on active les boutons de play et vitesse
    d_playButton->setEnabled(true);
    d_speedSelector->setEnabled(true);
}

void MainWindow::onCarHasReachEndOfPath()
{
    auto* car = qobject_cast<Car*>(sender());

    if(car == nullptr)
        return;

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

MainWindow::~MainWindow()
{
    delete d_mapWidget;
    delete d_rightSidebar;
    for(auto* child: menuBar()->children())
        delete child;
}
