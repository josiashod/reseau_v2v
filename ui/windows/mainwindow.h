#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../widgets/mapviewer.h"

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
    void onShowHideDescs(bool);

private:
    void creerInterface();
    MapViewer* d_map;

    bool d_showRoads = true;
    bool d_showWaters = true;
    bool d_showBuildings = true;
    bool d_showParks = true;
    bool d_showDescs = true;
};
#endif // MAINWINDOW_H
