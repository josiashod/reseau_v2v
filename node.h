#ifndef NODE_H
#define NODE_H

#include <QPointF>
#include <QString>

class Node: public QPointF
{
public:
    Node(long long, QPointF);
    Node(const Node&);

    long long id() const;
protected:
    long long d_id;
};

#endif // NODE_H
