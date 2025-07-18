#include "car.h"
#include "qpainter.h"
#include <random>
#include <QBrush>
#include <QPen>
#include <QColor>

QString colors[] = {"black", "blue", "red", "green", "yellow"};
size_t Car::d_instance_counter = 0;

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

/**
* @brief distance: calcule la distance entre la voiture et la position passée en paramètre
* @return la distance obtenue
*/
double distance(QPointF s, QPointF e)
{
    return std::hypot(s.x() - e.x(), s.y() - e.y());
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


Car::Car(QGraphicsItem *parent):
    QGraphicsObject{parent}, d_showFreq{true},
    d_v{20.0}, d_freq{30.5}, d_intensity{10}, d_acceleration{1.0}
    , d_from{0}, d_to{0}, d_path{std::vector<osm::Node*>(0)}, d_color{randomColor()}, d_pixmap{getRandomImage()}
{
    d_id = ++d_instance_counter;

    setData(0, QString::number(d_id));
    setFlags(QGraphicsItem::ItemIsSelectable);
    d_color.setAlphaF(0.5);

    setZValue(2);
}

// // Constructeur avec position, vitesse et fréquence
Car::Car( std::vector<osm::Node*>& path, double vitesse, double frequence, double intensity, QGraphicsItem *parent):
    QGraphicsObject{parent}, d_showFreq{true}, d_v{vitesse}, d_freq{frequence}, d_intensity{intensity}, d_acceleration{1.0}, d_from{0}, d_to{1}, d_path{path}
    , d_color{randomColor()}, d_pixmap{getRandomImage()}
{
    d_id = ++d_instance_counter;

    if(d_path.size() == 1)
    {
        d_to = 0;
    }
    if(!d_path.empty())
        setPos(d_path[0]->pos());

    setData(0, QString::number(d_id));
    setFlags(QGraphicsItem::ItemIsSelectable);
    d_color.setAlphaF(0.5);

    setZValue(2);
}

QRectF Car::boundingRect() const
{
    return QRectF(-d_freq - PEN_WIDTH / 2, -d_freq - PEN_WIDTH / 2,
                  d_freq*2 + PEN_WIDTH, d_freq*2 + PEN_WIDTH);
}

QPainterPath Car::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Car::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto originalBrush{painter->brush()};
    auto originalPen{painter->pen()};

    qreal pixWidth = d_pixmap.width();
    qreal pixHeight = d_pixmap.height();

    auto pen{originalPen};
    QBrush brush{d_color};
    pen.setWidth(PEN_WIDTH);

    painter->setPen(pen);
    painter->setBrush(brush);

    if(d_showFreq)
        painter->drawEllipse(boundingRect());
    painter->drawPixmap(-(pixWidth / 2), -(pixHeight / 2), d_pixmap);

    painter->setBrush(originalBrush);

    updateOrientation();
}

size_t Car::id() const
{
    return d_id;
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
    d_acceleration = acc;
}

osm::Node* Car::from() const
{
    return d_path[d_from];
}

osm::Node* Car::to() const
{
    return d_path[d_to];
}

void Car::updateOrientation()
{
    if(d_path.empty() || d_path.size() < d_to)
        return;

    QPointF d = d_path[d_to]->pos() - d_path[d_from]->pos();
    qreal angle = std::atan2(d.y(), d.x()) * 180 / M_PI;
    setRotation(angle + 90);
}

void Car::updateFrequenceVisibility(bool activated)
{
    d_showFreq = activated;
    update();
}

double Car::receivedPower(const QPointF& p) const
{
    // Distance entre le véhicule et un point donné
    double dist = distance(pos(), p);
    if (dist == 0) return d_intensity;  // Puissance maximale à la position du véhicule
    // Calcul de la perte FSPL et de la puissance reçue
    return d_intensity - fspl(dist, d_freq);
}

void Car::updateCoverage()
{
    double received_intensity = receivedPower({0, 0});
    if (received_intensity < d_power_threshold)
    {
        d_color.setAlphaF(0);
        return;
    }

    //    double scaling_factor = d_freq / 10.0;
    //    double rad = std::sqrt(received_intensity / d_power_threshold) * 4 * scaling_factor;
    //    rad = std::clamp(rad, 10.0, 200.0);

    d_color.setAlphaF(std::clamp(received_intensity / 100, 0.1, 0.55));
    update();
}

void Car::nextMove()
{
    d_from = d_to;
    if((d_to + 1) < d_path.size())
    {
        ++d_to;
    }
    setPos(d_path[d_from]->pos());
}

void Car::move(double interval)
{
    if(d_path[d_to] == d_path[d_from])
    {
        emit hasReachEndOfPath();
        return;
    }

    d_elapsed += interval / 1000.0;  // Convertir l'intervalle en secondes

    double time = duree(distance(d_path[d_from]->pos(), d_path[d_to]->pos()), d_v);

    updateConnections();

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

        auto position = d_path[d_from]->pos() + ((d_path[d_to]->pos() - d_path[d_from]->pos()) * progress);
        updateCoverage();
        setPos(position);
        update();
    }
}

void Car::setPath(const std::vector<osm::Node*>& path)
{
    d_path = path;
    d_from = d_to;
    d_to = 1;
    d_elapsed = 0.0;
    updateOrientation();
}

bool Car::isConnectedTo(const Car* other) const
{
    // Distance entre les deux voitures
    double dist =  distance(pos(), other->pos());
    double radius = std::max((d_freq * 0.5), (other->d_freq * 0.5));

    return dist <= radius;
}

void Car::clearConnections()
{
    d_connected_cars.clear();
    PEN_WIDTH = 1;
    update();
}

void Car::updateConnectionWith(Car* other)
{
    if(isConnectedTo(other))
    {
        PEN_WIDTH = 3;
        d_connected_cars.append(other);
        update();
    }
}

QString Car::toString() const
{
    QString str = QString("<p>Car N°%1: pos(%2, %3), speed: %4 Km/h, frequence: %5 Hz, puissance reçue: %6")
                      .arg(d_id)
                      .arg(pos().x())
                      .arg(pos().y())
                      .arg(d_v)
                      .arg(d_freq)
                      .arg(receivedPower({0, 0}));

    if(!d_connected_cars.empty())
    {
        str += "<br>Connected to cars: ";
        for (const Car* car : d_connected_cars)
        {
            str += "N° " + QString::number(car->id()) + ",";
        }
    }
    str.chop(1);
    return str + "</p>";
}

void Car::updateConnections()
{
    QList<QGraphicsItem*> collidingObjects = collidingItems(Qt::IntersectsItemShape);
    QVector<Car*> cars;

    for (auto* items : collidingObjects) {
        if(auto* car = dynamic_cast<Car*>(items))
        {
            cars.append(car);
        }
    }

    clearConnections();

    for (int i = 0; i < cars.size(); ++i) {
        updateConnectionWith(cars[i]);
    }
}

