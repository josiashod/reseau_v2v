#ifndef IDATASOURCE_H
#define IDATASOURCE_H

#include <vector>
#include <memory>
#include <map>
#include <QString>

namespace osm {
    class Node;
    class Way;
    class Edge;

    /**
     * @brief Structure pour représenter un nœud brut (sans allocation full)
     */
    struct NodeData {
        long long id;
        double lat;
        double lon;
    };

    /**
     * @brief Structure pour représenter un way brut
     */
    struct WayData {
        long long id;
        std::vector<long long> nodeIds;  // IDs des nœuds du way
        std::map<QString, QString> tags;
    };

    /**
     * @brief InDataSource - Interface abstraite pour accès aux données OSM
     * 
     * Permet de basculer entre deux implémentations sans modifier Graph/MapWidget:
     * - SQLDataSource: wrapper DBManager (existant)
     * - BinaryDataSource: lecteur fichiers binaires (futur)
     * 
     * Retourne des données brutes, Graph::buildFromDataSource() les transforme en Node/Edge
     */
    class InDataSource {
    public:
        virtual ~InDataSource() = default;

        /**
         * @brief Récupère tous les nœuds dans une bounding box
         */
        virtual std::vector<NodeData> getNodesInBounds(
            double minLat, double minLon, 
            double maxLat, double maxLon
        ) = 0;

        /**
         * @brief Récupère tous les ways dans une bounding box (par leurs nœuds)
         */
        virtual std::vector<WayData> getWaysInBounds(
            double minLat, double minLon,
            double maxLat, double maxLon
        ) = 0;

        /**
         * @brief Récupère les nœuds d'un way spécifique (par ID)
         */
        virtual std::vector<NodeData> getWayNodes(long long wayId) = 0;

        /**
         * @brief Récupère un nœud par ID exact
         */
        virtual NodeData* getNodeById(long long id) = 0;

        /**
         * @brief Récupère un way par ID exact
         */
        virtual WayData* getWayById(long long id) = 0;

        /**
         * @brief Récupère limites géographiques (bounds du fichier OSM)
         */
        virtual void getBounds(
            double& minLat, double& minLon,
            double& maxLat, double& maxLon
        ) const = 0;

        /**
         * @brief Nombre total de nœuds
         */
        virtual size_t nodeCount() const = 0;

        /**
         * @brief Nombre total de ways
         */
        virtual size_t wayCount() const = 0;

        /**
         * @brief Tous les ways (pour itération globale)
         */
        virtual std::vector<WayData> getAllWays() = 0;
    };

} // namespace osm

#endif // IDATASOURCE_H
