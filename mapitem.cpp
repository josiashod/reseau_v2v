#include "mapitem.h"


MapItem::MapItem(unsigned int id)
    : d_id{id}  
{}

void MapItem::addNode(Node* n) {
    if (n != nullptr) {
        d_nodes.push_back(n);
    }
}
void MapItem::addTag(const QString& key, const QString& value) {
    d_tags[key] = value; 
}

bool MapItem::tagExist(QString tag) const {
    return d_tags.find(tag) != d_tags.end();  
}
QString MapItem::tag(QString tag) const {
    auto it = d_tags.find(tag);
    if (it != d_tags.end()) {
        return it->second;
    }
    return QString();
}
unsigned int MapItem::nodeSize() const {
    return d_nodes.size();
}
