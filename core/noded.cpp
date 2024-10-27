#include "noded.h"

NodeD::NodeD(long long id, QPointF point, QString name): Node{id, point}, d_name{name}
{}

NodeD::NodeD(Node n, QString name): Node{n}, d_name{name}
{}

QString NodeD::name() const
{ return d_name; }
