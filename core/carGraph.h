#ifndef CARGRAPH_H
#define CARGRAPH_H

#include <unordered_map>
#include <vector>
#include <QPointF>
#include <memory>

namespace car {


    class CarEdge;
    class CarGraph;

    /**
     * @brief Classe représentant un nœud (voiture) dans le graphe.
    */
class CarNode {
public:
    /**
         * @brief Constructeur de la classe CarNode.
         * @param id Identifiant unique de la voiture.
         * @param x Position X de la voiture.
         * @param y Position Y de la voiture.
         * @param power Portée radio de la voiture.
    */
    CarNode(long long id, double x, double y, double power);

    /**
         * @brief Constructeur de copie.
    */
    CarNode(const CarNode &n);

    /**
         * @brief Retourne l'identifiant de la voiture.
         * @return Identifiant unique de la voiture.
    */
    long long id() const;

    /**
         * @brief Retourne la position de la voiture.
         * @return Coordonnées de la voiture sous forme QPointF.
    */
    QPointF pos() const;

    /**
         * @brief Retourne la puissance radio de la voiture.
         * @return Portée radio.
    */
    double power() const;

    /**
    * @brief Retourne les arêtes connectées à ce nœud.
    * @return Vecteur des arêtes.
    */
    std::vector<CarEdge*> getEdges() const;

    /**
         * @brief Vérifie si un autre nœud est un voisin direct.
         * @param n Pointeur vers le nœud à vérifier.
         * @return Vrai si le nœud est un voisin, faux sinon.
    */
    bool hasNeighbor(CarNode* n) const;

private:
    /**
         * @brief Ajoute une arête connectée à ce nœud.
         * @param e Pointeur vers l'arête à ajouter.
    */
    void addEdge(CarEdge* e);

    long long d_id;                      ///< Identifiant de la voiture.
    QPointF d_pos;                       ///< Position de la voiture.
    double d_power;                      ///< Portée radio de la voiture.
    std::vector<CarEdge*> d_edges;       ///< Arêtes connectées.
    std::vector<CarNode*> d_neighbors;   ///< Liste des voisins.

    friend class CarGraph; // Accès aux méthodes privées par la classe CarGraph.
    friend class CarEdge;  // Accès aux méthodes privées par la classe CarEdge.
};

    /**
     * @brief Classe représentant une arête (connexion entre deux voitures).
    */
class CarEdge {
public:
    /**
    * @brief Constructeur de la classe CarEdge.
    * @param start Nœud de départ.
    * @param end Nœud d'arrivée.
    */
    CarEdge(CarNode* start, CarNode* end);

    /**
    * @brief Retourne le nœud de départ.
    * @return Pointeur vers le nœud de départ.
    */
    CarNode* getStart() const;

    /**
    * @brief Retourne le nœud d'arrivée.
    * @return Pointeur vers le nœud d'arrivée.
    */
    CarNode* getEnd() const;

private:
    CarNode* d_start; ///< Nœud de départ.
    CarNode* d_end;   ///< Nœud d'arrivée.

    friend class CarGraph; // Accès aux méthodes privées par la classe CarGraph.
};

    /**
     * @brief Classe représentant le graphe des connexions entre voitures.
     */

class CarGraph {
public:
    /**
    * @brief Constructeur de la classe CarGraph.
    */
    CarGraph();

    /**
    * @brief Destructeur de la classe CarGraph.
    */

    ~CarGraph();

    /**
    * @brief Ajoute un nœud (voiture) au graphe.
    * @param id Identifiant de la voiture.
    * @param x Position X de la voiture.
    * @param y Position Y de la voiture.
    * @param power Portée radio de la voiture.
    * @return Pointeur vers le nœud ajouté.
    */
    CarNode* addNode(long long id, double x, double y, double power);

    /**
    * @brief Ajoute une arête entre deux nœuds.
    * @param start Pointeur vers le nœud de départ.
    * @param end Pointeur vers le nœud d'arrivée.
    */
    void addEdge(CarNode* start, CarNode* end);

    /**
    * @brief Met à jour les connexions entre voitures en fonction d'une portée.
    * @param distanceMax Distance maximale pour établir une connexion.
    */
    void updateConnections(double distanceMax);

    /**
    * @brief Efface toutes les arêtes du graphe.
    */
    void clearEdges();

    /**
         * @brief Trouve un nœud par son identifiant.
         * @param id Identifiant du nœud.
         * @return Pointeur vers le nœud trouvé ou nullptr si introuvable.
    */
    CarNode* findNode(long long id) const;

private:
    std::unordered_map<long long, std::unique_ptr<CarNode>> d_nodes; /// Liste des nœuds.
    std::vector<std::unique_ptr<CarEdge>> d_edges; ///< Liste des arêtes.
};
}

#endif // CARGRAPH_H

