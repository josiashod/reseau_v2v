# reseau_v2v

Simulation V2V sur une carte OpenStreetMap.

L'application lit maintenant directement les fichiers `.osm` pour afficher la carte et construire le graphe routier. La base de données n'est plus nécessaire pour le chargement/runtime de la carte.

## Fonctionnement actuel

Au lancement, l'application charge la carte par défaut embarquée dans les ressources Qt :

```text
:/lib/map.osm
```

Le fichier OSM est parsé en mémoire par `OsmReader::parseMapFile()` :

- les `node` sont stockés en mémoire par identifiant ;
- les `way` sont classés selon leurs tags ;
- les routes, bâtiments, cours d'eau et parcs sont dessinés dans leurs couches respectives ;
- les routes compatibles voiture alimentent le graphe de déplacement ;
- le calcul de chemin utilise A*.

## Changer De Carte

Depuis l'interface :

```text
Carte -> Charger une carte OSM
```

Sélectionner un fichier `.osm`. L'application nettoie la carte courante, vide le graphe, parse le nouveau fichier et reconstruit l'affichage directement depuis le fichier choisi.

## Éléments OSM Pris En Compte

Les éléments sont identifiés à partir des tags OSM :

- routes : `highway`
- bâtiments : `building`
- eau : `waterway`, `natural=water`, certains tags `water`
- parcs/espaces verts : `leisure`, `landuse`

Le graphe des voitures est construit uniquement à partir des routes acceptées par `Way::isCarWay()`.

## Compilation

Avant compilation, créer `env.cpp` à partir de `env.examples` si le fichier n'existe pas encore.

Le projet se compile avec Qt Creator ou avec `qmake` :

```bash
qmake reseau_v2v.pro
make
```

## Note Sur L'Ancienne Base De Données

Le code lié à `DBManager` et à l'ancien import OSM en base peut encore exister dans le projet, mais il n'est plus utilisé par `MapWidget` pour afficher la carte.

Le flux recommandé est désormais :

```text
fichier OSM -> parsing mémoire -> rendu Qt + graphe routier
```

au lieu de :

```text
fichier OSM -> base de données -> requêtes SQL -> rendu Qt
```
