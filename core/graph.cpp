#include "graph.h"
#include <iostream>
#include <cmath>

namespace osm {

// ======================== Classe Node Test ========================

//Node::Node(int id, double x, double y)
//    : d_id(id), d_coord(x, y) {}

//Node::Node(const Node &n)
//    : d_id(n.d_id), d_coord(n.d_coord), d_egdes(n.d_egdes) {}

//long long Node::getId() const {
//    return d_id;
//}

//std::vector<Edge*>& Node::getEdges() const {
//    return const_cast<std::vector<Edge*>&>(d_egdes);
//}

//void Node::addEdge(Edge *e) {
//    d_egdes.push_back(e);
//}

//Edge* Node::getRandomEdge() const {
//    if (d_egdes.empty()) return nullptr;
//    return d_egdes[rand() % d_egdes.size()];
//}

//bool Node::operator==(const Node& n) {
//    return d_id == n.d_id;
//}

// ======================== Classe Edge ========================

Edge::Edge(Node* start, Node* end)
    : d_start(start), d_end(end) {
    d_distance = distance();
    d_duree = d_distance;
    start->addEdge(this);
    end->addEdge(this);
}

Edge::Edge(const Edge& e)
    : d_start(e.d_start), d_end(e.d_end), d_distance(e.d_distance), d_duree(e.d_duree) {}

Node* Edge::findNode(long long id) const {
    if (d_start->getId() == id) return d_start;
    if (d_end->getId() == id) return d_end;
    return nullptr;
}

Node* Edge::getStart() const {
    return d_start;
}

Node* Edge::getEnd() const {
    return d_end;
}

bool Edge::operator==(const Edge& e) {
    return (*d_start == *(e.d_start) && *d_end == *(e.d_end)) ||
           (*d_start == *(e.d_end) && *d_end == *(e.d_start));
}

double Edge::distance() {
    double dx = d_start->d_coord.x() - d_end->d_coord.x();
    double dy = d_start->d_coord.y() - d_end->d_coord.y();
    return std::sqrt(dx * dx + dy * dy);
}

double Edge::duree() {
    return d_distance;
}

// ======================== Classe Graph Test ========================

//Node* Graph::addNode(int id, double x, double y) {
//    Node* newNode = new Node(id, x, y);
//    d_nodes.push_back(newNode);
//    return newNode;
//}

//Edge* Graph::addEdge(const Node* start, const Node* end) {
//    Edge* newEdge = new Edge(const_cast<Node*>(start), const_cast<Node*>(end));
//    d_edges.push_back(newEdge);
//    return newEdge;
//}

//// Méthode pour afficher le graphe
//void Graph::displayGraph() const {
//    std::cout << "=== Liste des Nœuds ===" << std::endl;
//    for (const auto& node : d_nodes) {
//        std::cout << "Node ID: " << node->getId() << " (" << node->d_coord.x() << ", " << node->d_coord.y() << ")\n";
//    }

//    std::cout << "\n=== Liste des Arêtes ===" << std::endl;
//    for (const auto& edge : d_edges) {
//        std::cout << "Edge entre Node " << edge->getStart()->getId()
//                  << " et Node " << edge->getEnd()->getId()
//                  << " - Distance: " << edge->distance() << std::endl;
//    }
//}

} // namespace osm
