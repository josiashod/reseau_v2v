#ifndef NODED_H
#define NODED_H

#include "node.h"

class NodeD : public Node
{
public:
    NodeD(long long, QPointF, QString);
    NodeD(Node, QString);

    /**
     * @brief name: Retourne le nom de description du noeud
     * @return QString
     */
    QString name() const;
private:
    QString d_name;
};

#endif // NODED_H
