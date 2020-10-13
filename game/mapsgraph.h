#ifndef MAPSGRAPH_H
#define MAPSGRAPH_H

#include <global.h>

#include "roomexit.h"

namespace CORE
{

class Map;
class Room;

class MapsGraph
{
public:
    MapsGraph();

    bool init(QVector<Map*> maps);

    bool getPath(const Room *a, const Room *b, Room** roomPath, RoomExit::RoomExitType &exitType) const;

private:
    struct Node;

    struct Edge
    {
        Edge() :
            room(Q_NULLPTR),
            exitRoom(Q_NULLPTR),
            exit(RoomExit::RoomExitType::EXIT_NORTH),
            fromNode(Q_NULLPTR),
            toNode(Q_NULLPTR)
        {}

        Room* room;
        Room* exitRoom;
        RoomExit::RoomExitType exit;

        // Nodi
        Node *fromNode;
        Node *toNode;
    };

    struct Node
    {
        Node() :
            map(Q_NULLPTR)
        {

        }

        typedef QPair<QMUD::StaticIdType, int> Id;

        Id id() const;

        Map* map;
        int group;

        // Archi
        QVector<Edge> edges;
    };

    QVector<const Edge *> path(const Node& nodeA, const Node& nodeB) const;

    QMap<Node::Id, Node> m_mapNodes;
    QMap<Node::Id, QMap<Node::Id, QVector<const Edge*>>> m_tablePaths;
};

}

#endif // MAPSGRAPH_H
