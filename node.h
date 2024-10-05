#ifndef NODE_H
#define NODE_H

#include <QPointF>
#include <QString>

class Node: public QPointF
{
public:
    Node(unsigned int, QPointF);
    Node(Node&);

    unsigned int id() const;
protected:
    unsigned int d_id;
};

#endif // NODE_H
