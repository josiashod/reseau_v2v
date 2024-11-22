#include "graph.h"
#include <random>
#include <stack>
#include <limits>
#include <queue>

#include <QDebug>

namespace osm
{

double distance(QPointF s, QPointF e)
{
    QPointF d = e - s;
    return std::sqrt((d.x() * d.x()) + (d.y() * d.y()));
}

Node::Node(long long id, double x, double y): d_id{id}, d_pos{x, y}, d_edges{}
{}
Node::Node(const Node &n): d_id{n.d_id}, d_pos{n.d_pos}, d_edges{n.d_edges}
{}
long long Node::id() const
{ return d_id; }

QPointF Node::pos() const
{ return d_pos; }

std::vector<Edge*> Node::getEdges() const
{ return d_edges; }

Edge* Node::getRandomEdge() const
{
    if (d_edges.empty())
        return nullptr;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, d_edges.size() - 1);

    return d_edges[dist(gen)];
}

Node* Node::getRandomNeighbor(Node* d_from) const
{
    if (d_neighbors.empty())
        return nullptr;

    if(d_neighbors.size() == 1)
        return d_neighbors[0].first;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, d_neighbors.size() - 1);

    int i = dist(gen);

    if(!d_from)
        d_neighbors[i].first;
    else
    {
        while(d_neighbors[i].first == d_from)
            i = dist(gen);
    }


    return d_neighbors[i].first;
}

void Node::addEdge(Edge *e)
{
    d_edges.push_back(e);
}

void Node::addNeighbor(Node* neighbor, double distance)
{
    d_neighbors.emplace_back(neighbor, distance);
}

bool Node::hasNeighbor(Node* n) const
{
    size_t i = 0;

    while(i < d_neighbors.size() && d_neighbors[i].first != n)
        ++i;

    return i != d_neighbors.size();
}

bool Node::operator==(const Node& n) const
{
    return (d_id == n.d_id) && std::equal(d_edges.begin(), d_edges.end(), n.d_edges.begin());
}

Node& Node::operator=(const Node& n)
{
    if(this != &n)
    {
        d_id = n.d_id;
        d_edges = n.d_edges;
        d_neighbors = n.d_neighbors;
        d_pos = n.d_pos;
    }

    return *this;
}

Edge::Edge(Node* start, Node* end): d_start{start}, d_end{end}
{
//    if (start)
//        d_start->addEdge(this);
//    if (end)
//        d_end->addEdge(this);
}

Edge::Edge(const Edge& e): d_start(e.d_start), d_end(e.d_end)
{}

Node* Edge::getStart() const
{
    return d_start;
}

Node* Edge::getEnd() const
{
    return d_end;
}

Node* Edge::destination(Node* source) const
{
    if(*source == *d_start)
        return d_end;
    else if (*source == *d_end)
        return d_start;

    return nullptr;
}

bool Edge::operator==(const Edge& e) {
    return (*d_start == *(e.d_start) && *d_end == *(e.d_end)) ||
    (*d_start == *(e.d_end) && *d_end == *(e.d_start));
}

Edge& Edge::operator=(const Edge& e)
{
    if(this != &e)
    {
        *d_start = *(e.d_start);
        *d_end = *(e.d_end);
    }

    return *this;
}

Graph::Graph():d_nodes{}, d_edges{}
{}

Graph::~Graph()
{
//    for(size_t i = 0; i < d_nodes.size(); ++i)
//        delete d_nodes[i];

//    for(size_t i = 0; i < d_edges.size(); ++i)
//        delete d_edges[i];
}


Node* Graph::findNode(long long id) const
{
    if(d_nodes.find(id) == d_nodes.end())
        return nullptr;

    return d_nodes.at(id).get();
}

Node* Graph::getRandomNode() const
{
    if(d_nodes.empty())
        return nullptr;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, d_nodes.size() - 1);

    auto it = d_nodes.begin();
    std::advance(it, dist(gen));

    return it->second.get();

//    return d_nodes.at(d_nodesId[index]).get();
}

Node* Graph::addNode(long long id, double x, double y)
{
    Node* n = findNode(id);

    if(!n)
    {
        d_nodes.emplace(id, std::make_unique<Node>(Node(id, x, y)));
//        d_nodesId.push_back(id);
        return d_nodes.at(id).get();
    }
    return n;
}

void Graph::addEdge(Node* start, Node* end)
{
    if (!start || !end)  // Vérifie la validité des nœuds.
        return;

//    d_edges.push_back(std::make_unique<Edge>(Edge(start, end)));
//    Edge* newEdge = d_edges.back().get();

    double dist = distance(start->d_pos, end->d_pos);

    start->addNeighbor(end, dist);
    end->addNeighbor(start, dist);
}


std::vector<Node*> Graph::dijkstraPath(long long startId, long long endId)
{
    std::unordered_map<long long, double> distances;
    std::unordered_map<long long, Node*> previous;
    std::vector<Node*> path;

    // Initialisation des distances avec l'infini
    for (const auto& pair : d_nodes) {
        distances[pair.first] = std::numeric_limits<double>::infinity();
    }
    distances[startId] = 0;

    // Comparateur pour la priority_queue
    auto cmp = [&distances](Node* left, Node* right) {
        return distances[left->id()] > distances[right->id()];
    };

    // Priority queue pour explorer les noeuds
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> queue(cmp);
    queue.push(d_nodes[startId].get());

    while (!queue.empty()) {
        Node* current = queue.top();
        queue.pop();

        // Arrêtez l'algorithme si on atteint le nœud cible
        if (current->id() == endId) {
            break;
        }

        for (const auto& neighbor : current->d_neighbors) {
            Node* neighborNode = neighbor.first;
            double weight = neighbor.second;
            double alternativeDist = distances[current->id()] + weight;

            if (alternativeDist < distances[neighborNode->id()]) {
                distances[neighborNode->id()] = alternativeDist;
                previous[neighborNode->id()] = current;
                queue.push(neighborNode);
            }
        }
    }

    // Reconstruire le chemin en remontant par les "previous"
    Node* step = d_nodes[endId].get();
    while (step) {
        path.push_back(step);
        if (step->id() == startId) break;
        step = previous[step->id()];
    }

    // Inverser le chemin pour partir de startId vers endId
    std::reverse(path.begin(), path.end());

    return path;
}
//    void Node::addEdge(Edge *e) {
//        d_egdes.push_back(e);
//    }

//    Edge* Node::getRandomEdge() const {
//        if (d_egdes.empty()) return nullptr;
//        int randomIndex = rand() % d_egdes.size();
//        return d_egdes[randomIndex];
//    }

//    bool Node::operator==(const Node& n) {
//        return d_id == n.d_id;
//    }

}
