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

Car::Car(): d_v{20.0}, d_freq{10.5}, d_intensity{10}, d_acceleration{1.0}
  , d_pos{0,0}, d_from{nullptr}, d_to{nullptr}
{}

// // Constructeur avec position, vitesse et fréquence
Car::Car( std::vector<osm::Node*>& path, double vitesse, double frequence, double intensity):
    d_v{vitesse}, d_freq{frequence}, d_intensity{intensity}, d_acceleration{1.0}, d_path{path}
{
    if(d_path.size() >= 2)
    {
        d_from = d_path[0];
        d_to = d_path[1];
    }
    else if(d_path.size() == 1)
    {
        d_from = d_path[0];
        d_to = d_from;
    }
    d_pos = d_from->pos();
    d_pixmap = new QGraphicsPixmapItem(getRandomImage());
    d_ellipse = new QGraphicsEllipseItem(0, 0, d_freq, d_freq);

    updateItem();
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
    osm::Node* destination = d_from->getRandomNeighbor();
    if(destination)
        d_to = destination;
    else
        d_to = d_from;
}


void Car::updateOrientation()
{
    d_pixmap->setTransformOriginPoint(d_pixmap->pixmap().width() / 2, d_pixmap->pixmap().height() / 2);
    QPointF d = d_to->pos() - d_from->pos();
    qreal angle = std::atan2(d.y(), d.x()) * 180 / M_PI;
    d_pixmap->setRotation(angle + 90);
}

void Car::nextMove()
{
//    osm::Node* destination = nullptr;
//    destination = d_to->getRandomNeighbor(d_from);

//    d_from = d_to;
//    d_to = destination;

//    d_pos = d_from->pos();
    d_from = d_to;
    if((++i) < d_path.size())
    {
        d_to = d_path[i];
    }
    d_pos = d_from->pos();
}

void Car::updateItem()
{
    qreal pixWidth = d_pixmap->pixmap().width();
    qreal pixHeight = d_pixmap->pixmap().height();

    d_pixmap->setTransformOriginPoint(pixWidth / 2, pixHeight / 2);

    auto carPos = d_pos - QPointF{pixWidth / 2, pixHeight / 2};
    d_pixmap->setPos(carPos);
    d_ellipse->setPos(d_pos - QPointF(d_freq / 2, d_freq / 2));
}

void Car::update(double interval)
{
    d_elapsed += interval / 1000.0;  // Convertir l'intervalle en secondes

    double time = duree(distance(d_from->pos(), d_to->pos()), d_v);

    // Vérifier si on est arrivée à destination
    if (d_elapsed > time)
    {
        d_from = d_to;
        d_elapsed = 0.0;
        nextMove();
        updateOrientation();
    }
    else
    {
        double progress = d_elapsed / time;

        d_pos = d_from->pos() + ((d_to->pos() - d_from->pos()) * progress);
        updateItem();
    }
}

osm::Node* Car::from() const
{
    return d_from;
}

osm::Node* Car::to() const
{
    return d_to;
}

void Car::updatePath(std::vector<osm::Node*>& path)
{
    i = 1;
    d_path = path;
    d_to = d_path[1];
    d_elapsed = 0.0;
    updateOrientation();
}

bool Car::hasReachedEndOfPath() const
{
    return d_to == d_from;
}

QString Car::toString() const
{
    QString str = "Car: pos:(), vitesse: % km/h, frequence: %f, puissance: %";

    return str;
}
