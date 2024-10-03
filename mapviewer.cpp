#include "mapviewer.h"

// CRÉATION DE L'INTERFACE

QSize MapViewer::sizeHint() const
{
    return {600, 200};
}

MapViewer::MapViewer(QWidget *parent)
    : QWidget{parent}
{
    QPalette pal = QPalette();

    pal.setColor(QPalette::Window, Qt::yellow);

    setAutoFillBackground(true);
    setPalette(pal);
}

void MapViewer::paintEvent(QPaintEvent *)
{
    QPainter painter{this};
    dessinePolygone(painter);
}

void MapViewer::dessinePolygone(QPainter &painter)
{
    static const QPointF points[4] = {
        QPointF(10.0, 80.0),
        QPointF(20.0, 10.0),
        QPointF(80.0, 30.0),
        QPointF(90.0, 70.0)
    };

    painter.drawPolygon(points, 4);
}

void MapViewer::map_update()
{
    update();
}
