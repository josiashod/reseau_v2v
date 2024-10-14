#include "way.h"

Way::Way(long long id): MapItem{id}
{}

void Way::draw(QGraphicsItemGroup* group, double scale_factor) const
{}

unsigned int MapItem::nodeSize() const {
    return d_nodes.size();
}
