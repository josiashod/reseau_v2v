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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onShowHideRoads(bool);
    void onShowHideBuildings(bool);
//    void onShowHideWaters(bool);
    void onShowHideParks(bool);
    void onShowHideLogs(bool);
    void onMapLoading(bool);
    void onMapLoaded(bool);
    void onPlay(bool);


    /**
     * @brief updatePlayButtonLabel met a jour le bouton play en fonction de l'état de la simutlation
     */
    void updatePlayButtonLabel();
    void updateSpeedSelector(int i);

    /**
     * @brief updateCarsPosition: update cars position
     */
    void updateCarsPosition();

private:
    void creerInterface();
    void createVComboBox();
    void selectSpeed();
    void playOrPause();
    void accelerate();

    MapWidget* d_mapView;
    LogWidget* d_logsView;
    QVBoxLayout* d_rightSidebar;
    QPushButton* d_playButton;
    QComboBox* d_speedSelector;

    int FPS = 60;
    QTimer *d_timer;

    osm::Graph d_graph;
    std::vector<std::unique_ptr<Car>> d_cars;

    bool d_showRoads = true;
    bool d_showWaters = true;
    bool d_showBuildings = true;
    bool d_showParks = true;
    bool d_showLogs = true;

    bool d_isPlaying = false;
    double d_v = 1.0;

    QStringList d_listOfSpeeds = {"0.25", "0.5" ,"0.75", "Normale", "1.25", "1.5", "1.75", "2"};
    double d_speeds[8] = {0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2};
    int d_selectedSpeed = 3;
};
#endif // MAINWINDOW_H
