#include "node.h"

Node::Node(unsigned int id, QPointF point): QPointF{point}, d_id{id}
{}

Node::Node(Node& n): QPointF{n.x(), n.y()}, d_id{n.d_id}
{}

unsigned int Node::id() const
{ return d_id; }
