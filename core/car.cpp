#include "car.h"
#include <random>

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

    int newWidth = static_cast<int>(pixmap.width() * 0.25);  // 80% de la largeur actuelle
    int newHeight = static_cast<int>(pixmap.height() * 0.25);
    pixmap = pixmap.scaled({newWidth, newHeight}, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return pixmap;
}

//    /**
//    * @brief distance: calcule la distance entre la voiture et la position passée en paramètre
//    * @return la distance obtenue
//    */
double distance(QPointF s, QPointF e)
{
    QPointF d = e - s;
    return std::sqrt((d.x() * d.x()) + (d.y() * d.y()));
}

double duree(double distance, double vitesse)
{
    return (distance / vitesse);
}

//    /**
//    * @brief duree: retourne la durée pour parcourir une distance donnée
//    * @param distance
//    * @return la durée obtenue
//    */
//    double duree(double distance) const;

//double Car::distance() const
//{
//    QPointF d = d_to - d_from;
//    return std::sqrt((d.x() * d.x()) + (d.y() * d.y()));
//}

//double Car::duree(double distance) const
//{
//    return (distance / d_v);
//}
Car::Car(): d_v{20.0}, d_freq{10.5}, d_intensity{10}, d_acceleration{1.0}
  , d_pos{0,0}, d_from{nullptr}, d_to{nullptr}
{}

// // Constructeur avec position, vitesse et fréquence
Car::Car(osm::Node* from, double vitesse, double frequence, double intensity):
    d_v{vitesse}, d_freq{frequence}, d_intensity{intensity}, d_acceleration{1.0}, d_from{from}
{
    if(d_from != nullptr)
    {
        d_pos = d_from->pos();
        d_pixmap = new QGraphicsPixmapItem(getRandomImage());
        d_pixmap->setPos(d_pos);
//        centerPixmap();

        randomDestination();

//        d_ellipse = new QGraphicsEllipseItem(d_pos.x(), d_pos.y(), d_freq, d_freq);
    }
}

//Car::Car(const Car &c): d_v{c.d_v}, d_freq{c.d_freq}, d_intensity{c.d_intensity},
//    d_acceleration{c.d_acceleration},  d_pos{c.d_pos}, d_from{c.d_from}, d_to{c.d_to}
//{
//    if(d_from != nullptr)
//    {
//        d_pixmap = new QGraphicsPixmapItem(getRandomImage());
//        d_pixmap->setPos(d_pos);
////        d_ellipse = new QGraphicsEllipseItem(d_pos.x(), d_pos.y(), d_freq, d_freq);
//    }
//}

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

QPointF Car::pos() const
{
    return d_pos;
}

double Car::acceleration() const
{
    return d_acceleration;
}

double Car::frequence() const
{
    return d_freq;
}

double Car::intensity() const
{
    return d_intensity;
}

void Car::accelerate(double acc)
{
    d_v = (d_v / d_acceleration) * acc;
    d_acceleration = acc;
}

QGraphicsPixmapItem* Car::pixmap() const
{
    return d_pixmap;
}

QGraphicsEllipseItem* Car::ellipse() const
{
    return d_ellipse;
}

void Car::randomDestination()
{
    std::pair<osm::Node*, double> destination = d_from->getRandomNeighbor();
    if(destination.first)
        d_to = destination.first;
    else
        d_to = d_from;

    QPointF d = d_to->pos() - d_from->pos();
    qreal angle = std::atan2(d.y(), d.x()) * 180 / M_PI;
    d_pixmap->setRotation(angle + 90);
}

void Car::nextMove()
{
//    std::swap(d_from, d_to);
std::pair<osm::Node*, double> destination = std::make_pair(nullptr, 0.0);
    do
    {
        destination = d_to->getRandomNeighbor();

    }while(!destination.first || destination.first == d_to);

//    if(destination.first)
//    {
        d_from = d_to;
        d_to = destination.first;
//    }
//    if(destination.first && destination.first != d_to)
//    {
//        d_from = d_to;
//        d_to = destination.first;
//    }
//    else
//    {
//        osm::Node* tmp = d_to;
//        d_to = d_from;
//        d_from = tmp;
//        qDebug() << "blocage";
//    }

//    qDebug() << "d_to: " << d_to->pos() << " d_from: " << d_from->pos();
    d_pos = d_from->pos();

    QPointF d = d_to->pos() - d_from->pos();
    qreal angle = std::atan2(d.y(), d.x()) * 180 / M_PI;
    d_pixmap->setRotation(angle + 90);
}

//void Car::centerPixmap()
//{
//    int labelWidth = d_pixmap->pixmap().width();
//    int labelHeight = d_pixmap->pixmap().height();

//    // Calculer la position pour que l'image soit centrée autour du point
//    qreal x = d_pos.x() - labelWidth / 2;
//    qreal y = d_pos.y() - labelHeight / 2;
//    d_pixmap->setPos(QPointF{x, y});
//}

void Car::update(double interval)
{
//    qDebug() << d_from->pos();
    d_elapsed += interval / 1000.0;  // Convertir l'intervalle en secondes

//    qDebug() << distance(from, to);
//    double time = duree(distance(from, to), d_v);
    double time = duree(distance(d_from->pos(), d_to->pos()), d_v);

//    qDebug() << time;

    // Vérifier si on est arrivée à destination
    if (d_elapsed > time)
    {
        d_from = d_to;
        d_elapsed = 0.0;
        nextMove();
    }
    else
    {
        double progress = d_elapsed / time;
//        QPointF pos = from + (to - from) * progress;
        d_pos = d_from->pos() + ((d_to->pos() - d_from->pos()) * progress);
        d_pixmap->setPos(d_pos);
//        centerPixmap();
//        d_ellipse->setPos(d_pos);
    }
}
