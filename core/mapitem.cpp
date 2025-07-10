#include "mapitem.h"


MapItem::MapItem(long long id)
    : d_id{id}  
{}

MapItem::MapItem(long long id, const std::vector<QPointF>& points)
    : d_id{id},
    d_points{points}
{}

long long MapItem::id() const
{ return d_id; }

void MapItem::addPoint(const QPointF& p) {
    d_points.push_back(p);
}

void MapItem::addTag(const QString& key, const QString& value) {
    d_tags[key] = value; 
}

bool MapItem::tagExist(const QString& tag) const {
    return d_tags.find(tag) != d_tags.end();  
}

QString MapItem::tag(const QString& tag) const {
    auto it = d_tags.find(tag);
    if (it != d_tags.end()) {
        return it->second;
    }
    return QString();
}

void MapItem::draw(QGraphicsItemGroup* group) const
{

}
