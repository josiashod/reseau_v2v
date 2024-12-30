#ifndef CARGRAPH_H
#define CARGRAPH_H

#include <unordered_map>
#include <vector>
#include <memory>
#include "car.h"

class CarGraph {
public:
    CarGraph();  // Constructeur par défaut.
    ~CarGraph(); // Destructeur.

    /**
     * @brief Ajoute une voiture au graphe.
     * @param car Voiture à ajouter.
     */
    void addCar(std::shared_ptr<Car> car);

    /**
     * @brief Met à jour les connexions entre voitures selon la puissance reçue.
     * @param speedOfLight Vitesse de la lumière (par défaut \( 3 \times 10^8 \)).
     * @param powerThreshold Seuil de puissance minimale pour établir une connexion.
     */
    void updateConnections(double speedOfLight = 3.0e8, double powerThreshold = 5.0);

    /**
     * @brief Efface toutes les connexions du graphe.
     */
    void clearConnections();

    /**
     * @brief Retourne la liste des voitures connectées à une voiture donnée.
     * @param carId Identifiant de la voiture.
     * @return Liste des identifiants des voitures connectées.
     */
    std::vector<size_t> getConnections(size_t carId) const;

private:
    struct Edge {
        size_t car1; ///< Identifiant de la première voiture.
        size_t car2; ///< Identifiant de la seconde voiture.
    };

    std::unordered_map<size_t, std::shared_ptr<Car>> d_cars; ///< Liste des voitures par identifiant.
    std::vector<Edge> d_edges; ///< Liste des connexions (arêtes).
};

#endif // CARGRAPH_H
