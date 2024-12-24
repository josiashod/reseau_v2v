#include "car.h"
#include <random>
#include <QBrush>
#include <QPen>
#include <QColor>

QString colors[] = {"black", "blue", "red", "green", "yellow"};
size_t Car::d_compteur_instance = 1;

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

QColor randomColor()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(1, 255);

    int r,g,b;
    do
    {
        r = dist(gen), g = dist(gen), b = dist(gen);
    }while((r + b + g) > 500);

    return QColor(r, g, b);
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

double fspl(double dist, double freq)
{
    const double C = -147.55;  // Constante pour des distances en mètres et des fréquences en Hz
    return (20 * std::log10(dist) + 20 * std::log10(freq) + C);
}

Car::Car(): d_v{20.0}, d_freq{10.5}, d_intensity{10}, d_acceleration{1.0}
  , d_pos{0,0}, d_from{nullptr}, d_to{nullptr}
{
    d_id = ++d_compteur_instance;
}

// // Constructeur avec position, vitesse et fréquence
Car::Car( std::vector<osm::Node*>& path, double vitesse, double frequence, double intensity):
    d_v{vitesse}, d_freq{frequence}, d_intensity{intensity}, d_acceleration{1.0}, d_path{path}
{
    d_id = ++d_compteur_instance;

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
    d_color = randomColor();
    d_ellipse = new QGraphicsEllipseItem(0, 0, d_freq, d_freq);

    updateItem();

    d_color.setAlphaF(0.5);

    QBrush brush(d_color);
    QPen pen(Qt::NoPen);
    d_ellipse->setBrush(brush);
    d_ellipse->setPen(pen);
    update_coverage();
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
}

void Car::update_coverage()
{
    double received_intensity = receivedPower({0, 0});
    if (received_intensity < d_power_threshold)
    {
        d_color.setAlphaF(0);
        d_ellipse->setBrush(d_color);
        return;
    }

    double scaling_factor = d_freq / 10.0;
    double rad = std::sqrt(received_intensity / d_power_threshold) * 4 * scaling_factor;
//    rad = std::clamp(rad, 10.0, 200.0);

    d_color.setAlphaF(std::clamp(received_intensity / 100, 0.1, 0.55));
    d_ellipse->setRect(0, 0, rad, rad);
    d_ellipse->setBrush(d_color);
    d_ellipse->setPos(d_pos - QPointF(rad / 2, rad / 2));
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
        update_coverage();
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

//QGraphicsEllipseItem* coverage = new QGraphicsEllipseItem(d_pos.x() - 50, d_pos.y() - 50, 100, 100);  // Ajuster la taille du cercle
//coverage->setPen(Qt::NoPen);  // Pas de bordure
//coverage->setBrush(QColor(255, 0, 0, static_cast<int>(255 * (power / d_intensity))));

double Car::receivedPower(const QPointF& pos) const
{
    // Distance entre le véhicule et un point donné
    double dist = distance(d_pos, pos);

    if (dist == 0) return d_intensity;  // Puissance maximale à la position du véhicule

    // Calcul de la perte FSPL et de la puissance reçue
    return d_intensity - fspl(dist, d_freq);
}

QString Car::toString() const
{
//        result += "\n\tConnected Cars:";
//        for (const Car* connectedCar : connectedCars) {
//            result += " " + QString::number(connectedCar->getId());
//        }
//        return result;
    QString str = QString("Car N°%1: posistion:(%2, %3), speed: (%4)km/h, frequence: %5 Hz, puissance r: %6")
            .arg(d_id)
            .arg(d_pos.x())
            .arg(d_pos.y())
            .arg(d_v)
            .arg(d_freq)
            .arg(receivedPower(d_pos));

    return str;
}
