#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "../widgets/mapwidget.h"
#include "../widgets/logwidget.h"

#include "../../core/car.h"
#include "../../core/graph.h"

class QPushButton;
class QComboBox;
class QVBoxLayout;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    // emit the new elapsed time
    void timeout(double);
    void freqVisibilityChanged(bool);

private slots:
    void onShowHideRoads(bool);
    void onShowHideBuildings(bool);
    void onShowHideFreq(bool);
    void onShowHideParks(bool);
    void onShowHideSidebar(bool);
    void onShowHideMesh(bool);
    void onMapLoading(bool);
    void onMapLoaded(bool);
    void onPlay();
    void onClearLog();
    void onAddCar();
    void addCar(int nb, double speed, double freq, double intensity);
    void onCarHasReachEndOfPath();
    void onMapRequestPause();
    /**
     * @brief updatePlayButtonLabel met a jour le bouton play en fonction de l'état de la simutlation
     */
    void updatePlayButtonLabel();
    void updateSpeedSelector(int i);
    /**
     * @brief updateFrame: update cars position
     */
    void updateFrame();

private:
    void creerInterface();
    void createVComboBox();
    void selectSpeed();
    void playOrPause();

    MapWidget* d_mapWidget;
    LogWidget* d_logsWidget;
    QWidget* d_rightSidebar;
    QPushButton* d_playButton;
    QPushButton* d_addCarButton;
    QComboBox* d_speedSelector;
    QLabel* d_timeLabel;
    QTimer d_timer;

    osm::Graph d_graph;

    bool d_showRoads;
    bool d_showCarFreq;
    bool d_showBuildings;
    bool d_showParks;
    bool d_showMesh;
    bool d_showSidebar;

    bool d_isPlaying;
    double d_v;

    double d_speeds[12] = {0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3};
    int d_selectedSpeed;
    int d_elapsed_time;
};
#endif // MAINWINDOW_H
