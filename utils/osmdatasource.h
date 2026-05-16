#ifndef OSMDATASOURCE_H
#define OSMDATASOURCE_H

#include "../core/indatasource.h"
#include <unordered_map>
#include <set>

/**
 * @brief OsmDataSource - Charge directement depuis fichier OSM (pas de SQL)
 * 
 * Parse le fichier OSM XML et stocke les données en mémoire.
 * Filtre les ways par type (roads, buildings, parks, waters).
 * Implémente InDataSource pour l'interface commune.
 */
class OsmDataSource : public osm::InDataSource {
public:
    OsmDataSource() = default;
    ~OsmDataSource() override = default;

    /**
     * @brief Charge le fichier OSM
     * @param filePath Chemin vers fichier .osm
     * @return true si succès
     */
    bool loadFromFile(const QString& filePath);

    std::vector<osm::NodeData> getNodesInBounds(
        double minLat, double minLon,
        double maxLat, double maxLon
    ) override;

    std::vector<osm::WayData> getWaysInBounds(
        double minLat, double minLon,
        double maxLat, double maxLon
    ) override;

    std::vector<osm::NodeData> getWayNodes(long long wayId) override;

    osm::NodeData* getNodeById(long long id) override;

    osm::WayData* getWayById(long long id) override;

    void getBounds(
        double& minLat, double& minLon,
        double& maxLat, double& maxLon
    ) const override;

    size_t nodeCount() const override;
    size_t wayCount() const override;
    
    std::vector<osm::WayData> getAllWays() override;

    // ===== Méthodes pour filtrer par type =====
    
    /**
     * @brief Récupère les ways qui sont des ROUTES (tag highway=*)
     */
    std::vector<osm::WayData> getRoads();

    /**
     * @brief Récupère les ways qui sont des BÂTIMENTS (tag building=*)
     */
    std::vector<osm::WayData> getBuildings();

    /**
     * @brief Récupère les ways qui sont des PARCS (tag leisure=park ou landuse=grass)
     */
    std::vector<osm::WayData> getParks();

    /**
     * @brief Récupère les ways qui sont de l'EAU (tag natural=water ou waterway=*)
     */
    std::vector<osm::WayData> getWaters();

    /**
     * @brief Vérifie si un way est une route (pour le graphe)
     */
    static bool isCarWay(const osm::WayData& way);

private:
    // Stockage en mémoire
    std::unordered_map<long long, osm::NodeData> d_nodes;
    std::unordered_map<long long, osm::WayData> d_ways;
    
    // Catégories filtrées
    std::set<long long> d_roads;      // Ways avec tag highway=*
    std::set<long long> d_buildings;  // Ways avec tag building=*
    std::set<long long> d_parks;      // Ways avec tag leisure=park, landuse=grass
    std::set<long long> d_waters;     // Ways avec tag natural=water, waterway=*
    
    // Bounds
    double d_minLat = 0.0, d_minLon = 0.0, d_maxLat = 0.0, d_maxLon = 0.0;
    bool d_boundsLoaded = false;
    
    /**
     * @brief Classifie un way selon ses tags
     */
    void classifyWay(const osm::WayData& way);
};

#endif // OSMDATASOURCE_H
