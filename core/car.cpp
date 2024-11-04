#include "car.h"
#include <QPropertyAnimation>

Car::Car(): d_v{50.0}, d_freq{1.5}, d_pos{0, {0,0}}
{}

Car::Car(const Node& pos, double vitesse, double frequence):
    d_v{vitesse}, d_freq{frequence}, d_pos{pos}, d_pixmap{new QGraphicsPixmapItem{QPixmap{"/home/josh/Téléchargements/car/taxi.png"}}}
{}

Car::Car(const Car &c): d_v{c.d_v}, d_freq{c.d_freq},
    d_pos{c.d_pos}
{}

double Car::distance(const Node& pos) const
{
    double x = (pos.x() - d_pos.x()), y = (pos.y() - d_pos.y());
    return std::sqrt((x*x) + (y*y));
}

double Car::duree(double distance) const
{
    return (d_v / distance);
}

QGraphicsPixmapItem* Car::pixmap() const
{
//    if(!d_pixmap)
//    {
//        QPixmap pixmap("/home/josh/Téléchargements/car/taxi.png");
//        d_pixmap = new QGraphicsPixmapItem(pixmap);
//    }

    return d_pixmap;
}

void Car::moveTo(const Node &pos)
{
////    distance(pos);

////    QGraphicsItem* d = new QGraphicsItem{};
//    // Initialisation de l'animation de la position
//    // Configuration de l'animation
//    QVariantAnimation* animation = new QVariantAnimation;
//    animation->setDuration(duree(distance(pos)) * 1000);
//    animation->setStartValue(pos);
//    animation->setEndValue(d_pos);

//    // Définir l'interpolation (optionnel, par exemple linéaire)
//    animation->setEasingCurve(QEasingCurve::Linear);

//    // Connecter l'animation à la position de l'élément
//    QObject::connect(animation, &QVariantAnimation::valueChanged, [this](const QVariant& value) {
//        d_pixmap->setPos(value.toPointF());
//        qDebug() << value.toPointF();
////        d_ellipse.setPos(value.toPointF());
//    });

//    // Démarrer l'animation
//    animation->start(QAbstractAnimation::DeleteWhenStopped);
    d_pos = pos;
}
