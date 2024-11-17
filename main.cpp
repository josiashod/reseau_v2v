#include "ui/windows/mainwindow.h"
#include "utils/osmreader.h"

#include <QApplication>
#include <QDebug>

//=========================TEST=========================
//#include "core/graph.h"

//void testGraph() {
//    using namespace osm;

//    // Création d'un graphe
//    Graph graph;

//    // Ajouter des nœuds
//    Node* n1 = graph.addNode(1, 0.0, 0.0);
//    Node* n2 = graph.addNode(2, 3.0, 4.0);
//    Node* n3 = graph.addNode(3, 6.0, 8.0);

//    qDebug() << "Nœuds créés:" << n1->getId() << n2->getId() << n3->getId();

//    // Ajouter des arêtes
//    graph.addEdge(n1, n2);
//    graph.addEdge(n2, n3);
//    graph.addEdge(n3, n1);

//    // Afficher le graphe en console
//    graph.displayGraph();
//}

//===========================FIN TEST=========================

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Enregistrement des types pour le système de méta-objets de Qt
    qRegisterMetaType<QVector<Water>>("QVector<Water>");
    qRegisterMetaType<QVector<Park>>("QVector<Park>");
    qRegisterMetaType<QMap<long long, Way>>("QMap<long long, Way>");
    qRegisterMetaType<QVector<Building>>("QVector<Building>");

    // Tester le graphe
    testGraph();

    // Démarrer la fenêtre principale
    MainWindow w;
    w.show();

    return a.exec();
}
