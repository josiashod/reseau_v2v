#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <QPointF>
#include <memory>

namespace osm
{
    class Edge;
    class Graph;

    class Node
    {
    public:
        /**
         * @brief Node Constructeur de copie
         * @param n
         */
        Node(const Node &n);

        /**
         * @brief getId retourne l'id du noeud
         * @return
         */
        long long id() const;

        /**
         * @brief pos
         * @return
         */
        QPointF pos() const;

        /**
         * @brief getEdges retourne les arrets lies aux noeuds
         * @return
         */
        std::vector<Edge*> getEdges() const;

        /**
         * @brief getRandomEdge Retourne un arrêt aleatoire
         * @return null si rien n'est trouvé
         */
        Edge* getRandomEdge() const;

        std::pair<Node*, double> getRandomNeighbor() const;

        bool hasNeighbor(Node* n) const;

        /**
         * @brief operator== compare deux noeuds entre eux
         * @param n: Node
         * @return
         */
        bool operator==(const Node& n) const;

        Node& operator=(const Node& n);

    private:
        /**
         * @brief Node Construit un noeud
         * @param id: id du noeud
         * @param x: abscisse
         * @param y: ordonnée
         */
        Node(long long id, double x, double y);
        /**
         * @brief addEdge Ajoute un arrets à la liste des arrets du noeuds
         * @param e: Egde
         */
        void addEdge(Edge *e);

        void addNeighbor(Node* neighbor, double distance);


        long long d_id;
        QPointF d_pos;
        std::vector<Edge*> d_edges;
        std::vector<std::pair<Node*, double>> d_neighbors;

        friend class Graph;
        friend class Edge;
    };

    class Edge{
    public:
        /**
         * @brief Edge: constructeur de copie
         * @param e
         */
        Edge(const Edge& e);
        /**
         * @brief getStart: retourne le noeud de depart
         * @return
         */
        Node* getStart()const;
        /**
         * @brief getEnd: retourne le noeud d'arrivée
         * @return
         */
        Node* getEnd()const;
        /**
         * @brief destination
         * @param source
         * @return
         */
        Node* destination(Node* source) const;
        /**
         * @brief operator == compare deux arret
         * @param e
         * @return
         */
        bool operator==(const Edge& e);
        /**
         * @brief operator =
         * @param e
         * @return
         */
        Edge& operator=(const Edge& e);
    private:
        /**
         * @brief Edge
         * @param start
         * @param end
         */
        Edge(Node* start, Node* end);

        Node* d_start;
        Node* d_end;
        friend class Graph;
    };

    class Graph
    {
    public:
        Graph();
        ~Graph();

        Node* findNode(long long id) const;
//        Edge* findEdge(long long id) const;
        Node* getRandomNode() const;
        Node* addNode(long long id, double x, double y);
        void addEdge(Node* start, Node* end);
    private:
        std::unordered_map<long long, std::unique_ptr<Node>> d_nodes;
        std::vector<long long> d_nodesId;
        std::vector<std::unique_ptr<Edge>> d_edges;
    };
}

#endif
