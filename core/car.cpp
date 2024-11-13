// #include "car.h"
// #include <cmath>
// #include <QGraphicsPolygonItem>
// #include <QPen>
// #include <QBrush>
// #include <QDebug>

// Car::Car() : d_v{50.0}, d_freq{1.5}, d_pos{0, {0, 0}} {}

// // Constructeur avec position, vitesse et fréquence
// Car::Car(const Node& pos, double vitesse, double frequence)
//     : d_v{vitesse}, d_freq{frequence}, d_pos{pos} {

//     QPolygonF triangle;
// //   triangle << QPointF(0, 0) << QPointF(-5, 10) << QPointF(5, 10);
//    triangle << QPointF(-5, -5) << QPointF(5, -5) << QPointF(5, 5) << QPointF(-5, 5);
//    d_polygon = new QGraphicsPolygonItem(triangle);
//    d_polygon->setBrush(QBrush(Qt::red)); // Remplir le triangle avec une couleur rouge
//    d_polygon->setPen(QPen(Qt::black));   // Contour du triangle en noir
//    d_polygon->setPos(d_pos);

//    // Ne pas utiliser le pixmap si vous utilisez un triangle
//    d_pixmap = nullptr;
// }

// Car::Car(const Car &c) : d_v{c.d_v}, d_freq{c.d_freq}, d_pos{c.d_pos} {}

// // Déplacer la voiture à une nouvelle position
// void Car::moveTo(const Node& pos) {
//     if (d_polygon) {
// //        qreal dx = pos.x() - d_pos.x();
// //        qreal dy = pos.y() - d_pos.y();
//         d_pos = pos;
// //        qreal angle = std::atan2(dy, dx) * 180 / M_PI;
// //        d_polygon->setRotation(angle);
//         d_polygon->setPos(d_pos.x(), d_pos.y());
//     }
// }


// QGraphicsPixmapItem* Car::pixmap() const {
//     return d_pixmap;
// }

// QGraphicsPolygonItem* Car::polygon() const {
//     return d_polygon;
// }

// double Car::distance(const Node& pos) const {
//     double x = (pos.x() - d_pos.x()), y = (pos.y() - d_pos.y());
//     return std::sqrt((x * x) + (y * y));
// }

// double Car::duree(double distance) const {
//     return (distance / d_v);
// }
