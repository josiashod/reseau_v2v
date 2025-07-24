#ifndef CAR_H
#define CAR_H

#include "graph.h"
#include <QGraphicsObject>
#include <QSet>

class Car: public QGraphicsObject
{
    Q_OBJECT
 public:
    Car(QGraphicsItem *parent = nullptr);
     /**
      * @brief Car avec paramètre
      * @param pos: la position de la voiture
      * @param vitesse: la vitesse de la voiture
      * @param frequence: la fréquence de la voiture
      */
    Car(std::vector<osm::Node*>& path, double vitesse = 50.0, double frequence = 60, double intensity = 25, QGraphicsItem *parent = nullptr);

    ~Car();

    size_t id() const;

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    /**
     * @brief draw: dessine la route
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    /**
     * @brief acceleration
     */
    double acceleration() const;
    /**
     * @brief frequence
     * @return
     */
    double frequence() const;
    /**
     * @brief intensity
     * @return
     */
    double intensity() const;
    /**
     * @brief accelerate
     * @param nv
     */
    void accelerate(double acc);
    /**
     * @brief updateOrientation update the direction of the car
     */
    void updateOrientation();
    /**
     * @brief updateCoverage update the radio coverage of the cars
     */
    void updateCoverage();
    /**
     * @brief receivedPower calculae the power received by the car
     * @param p the observaion point position
     * @return
     */
    double receivedPower(const QPointF& p) const;
    /**
     * @brief update the frequence visibility
     */
    void updateFrequenceVisibility(bool);
    /**
     * @brief nextMove move to the next segment of the path
     */
    void nextMove();
    void setPath(const std::vector<osm::Node*>& path);
    osm::Node* from() const;
    osm::Node* to() const;
    /**
     * @brief check if the car is connected to
     * @param other
     * @return
     */
    bool isConnectedTo(const Car* other) const;
    /**
     * @brief updateConnectionWith
     * @param other
     */
    void updateConnectionWith(Car* other);
    /**
     * @brief toString
     * @return
     */
    QString toString() const;

    /**
     * @brief fixed connections cars. Remove the cars with whom
     * i dont have connections anymore
     */
    void fixedConnections();

signals:
    // this event it's emit when the car has reach the end of a path
    void hasReachEndOfPath();
    /**
     * @brief this signals that the cars is connected to other cars
     */
    void isConnectedToCars();

    void requestSimulationPause(bool);

public slots:
    /**
     * @brief update mettre à jour la position de la voiture selon un interval
     * @param interval
     */
    void move(double interval);
    /**
     * @brief this paly the information of the car in a QMessageBox
     */
    void handleInfo();

private:
    static size_t d_instance_counter;
    static constexpr double d_power_threshold = 5.0;

    bool d_showFreq;
    // vitesse de la voiture
    double d_v;
    // fréquence
    double d_freq;
    // puissance de la voiture
    double d_intensity;
    // vitesse de lecture dans l'animation
    double d_acceleration;
    // position de depart de la voiture
    size_t d_from;
    // position de position d'arrivée
    size_t d_to;
    // Chemin a parcourir
    std::vector<osm::Node*> d_path;
    // Temps écoulé dans l'animation
    double d_elapsed = 0.0;
    // id du car
    size_t d_id;
    // couleur du rayon de frequence
    QColor d_color;
    // image de la voiture
    QPixmap d_pixmap;
    // connected cars
    QSet<Car*> d_connected_cars;

    qreal PEN_WIDTH = 1;
 };

#endif // CAR_H
