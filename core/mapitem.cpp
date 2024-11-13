#include "mapitem.h"


MapItem::MapItem(long long id)
    : d_id{id}  
{}

long long MapItem::id() const
{ return d_id; }

void MapItem::addPoint(const QPointF& p) {
    d_points.push_back(p);
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
