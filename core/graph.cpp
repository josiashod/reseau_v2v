#include "graph.h"
#include <cmath> // pour std::hypot
#include <cstdlib> // pour rand()

namespace osm
{
Graph::Graph() : d_nodes{}, d_edges{}
{}

Node* Graph::addNode(int id, double x, double y)
{
    Node* nouveauNoeud = new Node(id, x, y);
    d_nodes.push_back(nouveauNoeud);
    return nouveauNoeud;
}

Edge* Graph::addEdge(const Node* start, const Node* end)
{
    Edge* nouvelArret = new Edge(const_cast<Node*>(start), const_cast<Node*>(end));
    d_edges.push_back(nouvelArret);
    return nouvelArret;
}

// === Implémentation de Node ===

Node::Node(int id, double x, double y)
    : d_id(id), d_coord(x, y), d_egdes() {}

Node::Node(const Node &n)
    : d_id(n.d_id), d_coord(n.d_coord), d_egdes(n.d_egdes) {}

long long Node::getId() const {
    return d_id;
}

std::vector<Edge*>& Node::getEdges() const {
    return const_cast<std::vector<Edge*>&>(d_egdes); // Cast pour permettre de retourner une référence modifiable
}

void Node::addEdge(Edge *e) {
    d_egdes.push_back(e);
}

Edge* Node::getRandomEdge() const {
    if (d_egdes.empty()) return nullptr;
    int randomIndex = rand() % d_egdes.size();
    return d_egdes[randomIndex];
}

bool Node::operator==(const Node& n) {
    return d_id == n.d_id;
}

// === Implémentation de Edge ===

// Edge::Edge(Node* start, Node* end)
//     : d_start(start), d_end(end), d_distance(0), d_duree(0) {
//     // Calculer la distance et la durée à l'initialisation
//     d_distance = distance();
//     d_duree = duree();
// }

// Edge::Edge(const Edge& e)
//     : d_start(e.d_start), d_end(e.d_end), d_distance(e.d_distance), d_duree(e.d_duree) {}

// Node* Edge::findNode(long long id) const {
//     if (d_start->getId() == id) return d_start;
//     if (d_end->getId() == id) return d_end;
//     return nullptr;
// }

// Node* Edge::getStart() const {
//     return d_start;
// }

// Node* Edge::getEnd() const {
//     return d_end;
// }

// bool Edge::operator==(const Edge& e) {
//     return (d_start == e.d_start && d_end == e.d_end);
// }

// double Edge::distance() {
//     return std::hypot(d_end->x - d_start->x, d_end->y - d_start->y);
// }
// double Edge::duree() {
//     double averageSpeed = 50.0; // Supposons une vitesse moyenne de 50 unités
//     return distance() / averageSpeed;
// }

// === Implémentation de Graph ===

// Graph::Graph() : d_nodes(), d_edges() {}

// Node* Graph::addNode(int id, double x, double y) {
//     Node* newNode = new Node(id, x, y);
//     d_nodes.push_back(newNode);
//     return newNode;
// }

// Edge* Graph::addEdge(const Node* start, const Node* end) {
//     if (!start || !end) return nullptr; // Vérifie si les nœuds sont valides
//     Edge* newEdge = new Edge(const_cast<Node*>(start), const_cast<Node*>(end));
//     d_edges.push_back(newEdge);

//     // Ajouter l'arête aux nœuds de départ et d'arrivée
//     const_cast<Node*>(start)->addEdge(newEdge);
//     const_cast<Node*>(end)->addEdge(newEdge);

//     return newEdge;
// }

} // namespace osm
