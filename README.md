# reseau_v2v
# Indication

Avant toute compilation

Creer le fichier `env.cpp` en vous basant sur le contenu de `env.exemple`

Enssuite en fonction de la base de donné utilisé, executer ce script de la bd
```sql
-- Table des limites géographiques du fichier OSM
CREATE TABLE bounds (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    minlat REAL NOT NULL,
    minlon REAL NOT NULL,
    maxlat REAL NOT NULL,
    maxlon REAL NOT NULL
);

-- Table des nœuds (points géographiques)
CREATE TABLE nodes (
    id INTEGER PRIMARY KEY,  -- L'ID vient directement du fichier OSM
    lat REAL NOT NULL,
    lon REAL NOT NULL
);

-- Table des chemins (ways), regroupements de nœuds
CREATE TABLE ways (
    id INTEGER PRIMARY KEY  -- L'ID vient aussi du fichier OSM
);

-- Table associant les nœuds à un chemin (ordre implicite par insertion)
CREATE TABLE way_node (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    way_id INTEGER NOT NULL,
    node_id INTEGER NOT NULL,
    FOREIGN KEY (way_id) REFERENCES ways(id),
    FOREIGN KEY (node_id) REFERENCES nodes(id)
);

-- Table des tags (attributs associés aux nœuds ou chemins)
CREATE TABLE tags (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    element_type TEXT NOT NULL CHECK (element_type IN ('node', 'way')),
    element_id INTEGER NOT NULL,
    t_key TEXT NOT NULL,
    t_value TEXT NOT NULL
);
```
