#include "mapitem.h"
#include "dbmanager.h"

MapItem::MapItem(long long id): d_id{id}, d_nodes{}
{}

void MapItem::addNode(const Node& n)
{ d_nodes.push_back(n); }


