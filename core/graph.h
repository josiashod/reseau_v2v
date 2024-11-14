#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include<QPointF>

namespace osm
{
    class Edge;
    class Node
    {
    public:
        /**
         * @brief Node Construit un noeud
         * @param id: id du noeud
         * @param x: abscisse
         * @param y: ordonnée
         */
        Node(int id, double x, double y);

        /**
         * @brief Node Constructeur de copie
         * @param n
         */
        Node(const Node &n);

        /**
         * @brief getId retourne l'id du noeud
         * @return
         */
        long long getId() const;

        /**
         * @brief getEdges retourne les arrets lies aux noeuds
         * @return
         */
        std::vector<Edge*>& getEdges() const;

        /**
         * @brief addEdge Ajoute un arrets à la liste des arrets du noeuds
         * @param e: Egde
         */
        void addEdge(Edge *e);

        /**
         * @brief getRandomEdge Retourne un arrêt aleatoire
         * @return null si rien n'est trouvé
         */
        Edge* getRandomEdge() const;

        /**
         * @brief operator== compare deux noeuds entre eux
         * @param n: Node
         * @return
         */
        bool operator==(const Node& n);

    private:
        long long d_id;
        QPointF d_coord;
        std::vector<Edge*> d_egdes;
    };

    class Edge{
    public:
        /**
         * @brief Edge
         * @param start
         * @param end
         */
        Edge(Node* start, Node* end);
        /**
         * @brief Edge: constructeur de copie
         * @param e
         */
        Edge(const Edge& e);

        Node* findNode(long long id) const;
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
         * @brief operator == compare deux arret
         * @param e
         * @return
         */
        bool operator==(const Edge& e);
    private:

        double distance();
        double duree();

        Node* d_start;
        Node* d_end;
        double d_distance;
        double d_duree;
    };

    class Graph
    {
    public:
        Graph():d_nodes{}, d_edges{}
        {};

        Node* addNode(int id, double x, double y);
        Edge* addEdge(const Node* start, const Node* end);
    private:
        std::vector<Node*> d_nodes;
        std::vector<Edge*> d_edges;
    };
}

#endif
