#include "node.h"

Node::Node(long long id, QPointF point): QPointF{point}, d_id{id}
{}

Node::Node(const Node& n): QPointF{n.x(), n.y()}, d_id{n.d_id}
{}

long long Node::id() const
{ return d_id; }
