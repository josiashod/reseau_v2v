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

    menuBar()->addMenu("Vue");
    menuBar()->addMenu("Log");


    auto mainWidget {new QWidget{this}};
    auto *mainHlayout = new QHBoxLayout();
    mainHlayout->setContentsMargins(0, 0, 0, 0);
    mainWidget->setLayout(mainHlayout);
    setCentralWidget(mainWidget);

    d_map = new MapViewer{this};
    mainHlayout->addWidget(d_map, 1);
    mainHlayout->addWidget(new QLabel{"test"});
}

MainWindow::~MainWindow()
{
}
