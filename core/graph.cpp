#include "graph.h"

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
}
