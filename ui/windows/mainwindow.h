#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../widgets/mapwidget.h"
#include "../widgets/logwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onShowHideRoads(bool);
    void onShowHideBuildings(bool);
    void onShowHideWaters(bool);
    void onShowHideParks(bool);
    void onShowHideLogs(bool);
    void onMapLoading(bool);
    void onMapLoaded(bool);

private:
    void creerInterface();
    MapWidget* d_mapView;
    LogWidget* d_logsView;

    bool d_showRoads = true;
    bool d_showWaters = true;
    bool d_showBuildings = true;
    bool d_showParks = true;
    bool d_showLogs = true;
};
#endif // MAINWINDOW_H
