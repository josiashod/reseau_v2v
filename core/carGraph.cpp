#include "cargraph.h"
#include <cmath> // Pour std::pow, std::hypot

CarGraph::CarGraph() = default;
CarGraph::~CarGraph() = default;

void CarGraph::addCar(std::shared_ptr<Car> car) {
    d_cars[car->id()] = std::move(car);
}

void CarGraph::clearConnections() {
    d_edges.clear();
}

void CarGraph::updateConnections(double speedOfLight, double powerThreshold) {
    clearConnections(); // efface les anciennes connexion

    // ce qui suit est en fonction de la formule vu au cours Pr = (λ² * Pt * Gt * Gr) / (4 * π * d)².

    for (const auto& [id1, car1] : d_cars) {
        for (const auto& [id2, car2] : d_cars) {
            if (id1 != id2) {
                // Calcul de la distance entre les deux voitures.
                double distance = std::hypot(
                    car1->pos().x() - car2->pos().x(),
                    car1->pos().y() - car2->pos().y()
                    );

                // Calcul de la longueur d'onde λ.
                double wavelength = speedOfLight / car1->frequence();

                // Calcul de la puissance reçue Pr.
                double powerReceived = (std::pow(wavelength, 2) * car1->intensity() * car2->intensity()) /
                                       (std::pow(4 * M_PI * distance, 2));

                // Vérifie si la puissance reçue dépasse le seuil dynamique.
                if (powerReceived >= powerThreshold) {
                    d_edges.push_back({id1, id2});
                }
            }
        }
    }
}

std::vector<size_t> CarGraph::getConnections(size_t carId) const {
    std::vector<size_t> connections;
    for (const auto& edge : d_edges) {
        if (edge.car1 == carId) {
            connections.push_back(edge.car2);
        } else if (edge.car2 == carId) {
            connections.push_back(edge.car1);
        }
    }
    return connections;
}
