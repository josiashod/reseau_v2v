 #include "car.h"
#include <random>
// #include <cmath>
// #include <QGraphicsPolygonItem>
// #include <QPen>
// #include <QBrush>
// #include <QDebug>

QString colors[] = {"black", "blue", "red", "green", "yellow"};

QPixmap getRandomImage()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist_color(0, 4);
    std::uniform_int_distribution<> dist(1, 3);

    QString color = colors[dist_color(gen)];
    QString image = ":lib/img/cars/car_" + color + "_" + QString::number(dist(gen)) + ".png";

    QPixmap pixmap{};
    pixmap.load(image);

    if (pixmap.isNull())
    {
        pixmap.load(":lib/img/cars/car_black_1.png");
    }

    int newWidth = static_cast<int>(pixmap.width() * 0.45);  // 80% de la largeur actuelle
    int newHeight = static_cast<int>(pixmap.height() * 0.45);
    pixmap = pixmap.scaled({newWidth, newHeight}, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return pixmap;
}

Car::Car() : d_v{50.0}, d_freq{1.5}, d_from{0, 0}
{
    d_pixmap = new QGraphicsPixmapItem(getRandomImage());
    d_pixmap->setPos(d_from);
}

// // Constructeur avec position, vitesse et fréquence
Car::Car(const QPointF& pos, double vitesse, double frequence): d_v{vitesse}, d_freq{frequence}, d_from{pos}
{
//     QPolygonF triangle;
// //   triangle << QPointF(0, 0) << QPointF(-5, 10) << QPointF(5, 10);
//    triangle << QPointF(-5, -5) << QPointF(5, -5) << QPointF(5, 5) << QPointF(-5, 5);
//    d_polygon = new QGraphicsPolygonItem(triangle);
//    d_polygon->setBrush(QBrush(Qt::red)); // Remplir le triangle avec une couleur rouge
//    d_polygon->setPen(QPen(Qt::black));   // Contour du triangle en noir
//    d_polygon->setPos(d_from);
    // Ne pas utiliser le pixmap si vous utilisez un triangle
    d_pixmap = new QGraphicsPixmapItem(getRandomImage());
    d_pixmap->setPos(d_from);
}

Car::Car(const Car &c) : d_v{c.d_v}, d_freq{c.d_freq}, d_from{c.d_from}
{
    d_pixmap = new QGraphicsPixmapItem(getRandomImage());
    d_pixmap->setPos(d_from);
}

// // Déplacer la voiture à une nouvelle position
// void Car::moveTo(const Node& pos) {
//     if (d_polygon) {
// //        qreal dx = pos.x() - d_from.x();
// //        qreal dy = pos.y() - d_from.y();
//         d_from = pos;
// //        qreal angle = std::atan2(dy, dx) * 180 / M_PI;
// //        d_polygon->setRotation(angle);
//         d_polygon->setPos(d_from.x(), d_from.y());
//     }
// }


QGraphicsPixmapItem* Car::pixmap() const
{
    return d_pixmap;
}

QPointF Car::pos() const
{
    return d_from;
}

double Car::distance() const
{
    QPointF d = d_to - d_from;
    return std::sqrt((d.x() * d.x()) + (d.y() * d.y()));
}

double Car::duree(double distance) const
{
    return (distance / d_v);
}

void Car::update(double interval)
{
    d_elapsed += interval / 1000.0;  // Convertir l'intervalle en secondes
    double time = duree(distance());
    QPointF newPos;

    QPointF d = d_to - d_from;
    qreal angle = std::atan2(d.y(), d.x()) * 180 / M_PI;
    d_pixmap->setRotation(angle + 90);

    // Vérifier si on est arrivée à destination
    if (d_elapsed > time)
    {
        d_from = d_to;
        d_elapsed = 0.0;
        newPos = d_to;
    }
    else
    {
        double progress = d_elapsed / time;
        newPos = d_from + (d_to - d_from) * progress;
    }

    d_pixmap->setPos(newPos);
}

// QGraphicsPolygonItem* Car::polygon() const {
//     return d_polygon;
// }
