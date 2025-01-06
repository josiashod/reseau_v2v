#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../widgets/mapwidget.h"
#include "../widgets/logwidget.h"

#include "../../core/car.h"
#include "../../core/graph.h"

class QPushButton;
class QComboBox;
class QVBoxLayout;
class QTimer;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onShowHideRoads(bool);
    void onShowHideBuildings(bool);
    void onShowHideFreq(bool);
    void onShowHideParks(bool);
    void onShowHideSidebar(bool);
    void onShowHideMesh(bool);
    void onMapLoading(bool);
    void onMapLoaded(bool);
    void onPlay(bool);
    void onClearLog();
    void onAddCar();
    void addCar(int nb, double speed, double freq, double intensity);


    /**
     * @brief updatePlayButtonLabel met a jour le bouton play en fonction de l'état de la simutlation
     */
    void updatePlayButtonLabel();
    void updateSpeedSelector(int i);

//    void addCar(int nb, double freq, double intensity);

    /**
     * @brief updateCarsPosition: update cars position
     */
    void updateCarsPosition();

//    void onAddCarToPartialSelection(int id);
//    void onRemoveCarFromPartialSelection(int id);
//    void onDeletePartialSelection();
//    void onPersistSelection();

private:
    void creerInterface();
    void createVComboBox();
    void selectSpeed();
    void playOrPause();
    void accelerate();

    MapWidget* d_mapView;
    LogWidget* d_logsView;
    QWidget* d_rightSidebar;
    QPushButton* d_playButton;
    QPushButton* d_addCarButton;
    QComboBox* d_speedSelector;
    QLabel* d_timeLabel;

    int FPS = 80;
    QTimer *d_timer;

    osm::Graph d_graph;
    std::vector<std::unique_ptr<Car>> d_cars;
    // les voitures partiellement selectionnées par l'utilisateur
//    std::vector<int> d_car_partially_selected = std::vector<int>(0);
    // les voitures dont la selection a été validé
//    std::vector<int> d_car_selection = std::vector<int>(0);

    bool d_showRoads;
    bool d_showCarFreq;
    bool d_showBuildings;
    bool d_showParks;
    bool d_showMesh;
    bool d_showSidebar;

    bool d_isPlaying;
    double d_v;

    QStringList d_listOfSpeeds = {"0.25", "0.5" ,"0.75", "Normale", "1.25", "1.5", "1.75", "2", "2.25", "2.5", "2.75", "3"};
    double d_speeds[12] = {0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3};
    int d_selectedSpeed;
    int d_elapsed_time;
};
#endif // MAINWINDOW_H
