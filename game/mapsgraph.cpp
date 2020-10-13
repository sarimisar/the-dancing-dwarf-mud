#include "mapsgraph.h"
#include "map.h"
#include "room.h"
#include "globaldebug.h"

using namespace CORE;

//#define DEBUG_MAPS_GRAPH_GENERATION

MapsGraph::MapsGraph()
{

}

bool MapsGraph::init(QVector<Map *> maps)
{
    m_mapNodes.clear();

    for (auto map : maps)
    {
        QVector<Edge> mapExitsData;
        auto rooms = map->rooms();

        // Cerco tutte le uscite che danno su una mappa diversa o su un gruppo
        // diverso
        for (auto room : rooms)
        {
            auto exits = room->exits();

            for (auto exit : exits)
            {
                if (exit->exitRoom()->group() != room->group() ||
                    exit->exitRoom()->map() != map)
                {
                    Edge edge;
                    edge.room = room;
                    edge.exitRoom = exit->exitRoom();
                    edge.exit = exit->exitType();
                    mapExitsData.push_back(edge);

#ifdef DEBUG_MAPS_GRAPH_GENERATION
                    GLOBAL_INFO(QString(" >> %1:%2 [%3:%4] -> %5:%6 [%7:%8]")
                                .arg(map->name())
                                .arg(room->group())
                                .arg(room->name())
                                .arg(room->staticId())
                                .arg(edge.exitRoom->map()->name())
                                .arg(edge.exitRoom->group())
                                .arg(edge.exitRoom->name())
                                .arg(edge.exitRoom->staticId()));
#endif
                }
            }
        }

        // Divido le uscite per il gruppo della stanza a cui currispondono
        QMap<int, QVector<Edge>> mapExitsDataByGroup;

        for (auto exitData : mapExitsData)
            mapExitsDataByGroup[exitData.room->group()].push_back(exitData);

        // Per ogni gruppo identificato aggiungo uno nodo alla lista
        // dei nodi
        for (auto it = mapExitsDataByGroup.begin(); it != mapExitsDataByGroup.end(); ++it)
        {
            Node node;
            node.map = map;
            node.group = it.key();
            node.edges = it.value();
            node.edges.squeeze();

            m_mapNodes[node.id()] = node;
        }

        // Aggiungo i nodi corrispondenti ai gruppi senza uscite, sostanzialmente
        // quelli non presenti in m_mapNodes per la mappa corrente
        for (int i=0; i< map->groupCount(); ++i)
        {
            if (m_mapNodes.find(Node::Id(map->staticId(), i)) == m_mapNodes.end())
            {
                Node node;
                node.map = map;
                node.group = i;
                node.edges.clear();
                node.edges.squeeze();

                m_mapNodes[node.id()] = node;
            }
        }
    }

    // Valorizzo il mapGroupData degli exitData
    for (auto& node0 : m_mapNodes)
    {
        for (auto& edge : node0.edges)
        {
            edge.fromNode = &node0;

            for (auto& node1 : m_mapNodes)
            {
                if (node1.map == edge.exitRoom->map() &&
                    node1.group == edge.exitRoom->group())
                {
                    edge.toNode = &node1;
                    break;
                }
            }

            if (!edge.toNode)
            {
                GLOBAL_CRITICAL(QObject::tr(" >> Manca il nodo per la mappa [%1:%2]")
                                .arg(edge.exitRoom->map()->name())
                                .arg(edge.exitRoom->map()->staticId()));
                return false;
            }
        }
    }

    // Valorizzo la tabella dei path
    for (auto& nodeA : m_mapNodes)
        for (auto& nodeB : m_mapNodes)
            m_tablePaths[nodeA.id()][nodeB.id()] = path(nodeA, nodeB);

    return true;
}

bool MapsGraph::getPath(const Room * a, const Room * b, Room **roomPath, RoomExit::RoomExitType& exitType) const
{
    if (a->map() == b->map() && a->group() == b->group())
    {
        roomPath = Q_NULLPTR;
        return true;
    }

    auto itA = m_tablePaths.find(Node::Id(a->map()->staticId(), a->group()));

    if (itA == m_tablePaths.end())
        return false;

    auto itB = itA.value().find(Node::Id(b->map()->staticId(), b->group()));

    if (itB == itA.value().end())
        return false;

    if (itB.value().isEmpty())
        return false;

    *roomPath = itB.value()[0]->room;
    exitType = itB.value()[0]->exit;
    return true;
}

MapsGraph::Node::Id MapsGraph::Node::id() const
{
    return Id(map->staticId(), group);
}

QVector<const MapsGraph::Edge*> MapsGraph::path(const MapsGraph::Node &nodeA, const MapsGraph::Node &nodeB) const
{
    QMap<const Node*, int> nodeDistance;
    QMap<const Node*, const Node*> nodePrevNode;
    QMap<const Node*, const Edge*> nodePrevExit;
    QVector<const Node*> nodesQ;

    for (auto& mapGroupData : m_mapNodes)
    {
        nodesQ.push_back(&mapGroupData);
        nodeDistance[&mapGroupData] = INT_MAX;
        nodePrevNode[&mapGroupData] = Q_NULLPTR;
        nodePrevExit[&mapGroupData] = Q_NULLPTR;
    }

    nodeDistance[&nodeA] = 0;

    while (!nodesQ.isEmpty())
    {
        int index = 0;
        const Node* u = nodesQ[index];

        for (int i = 1; i < nodesQ.size(); ++i)
        {
            if (nodeDistance[nodesQ[i]] < nodeDistance[u])
            {
                u = nodesQ[i];
                index = i;
            }
        }

        nodesQ.removeAt(index);

        if (nodeDistance[u] == INT_MAX)
            break;

        for (auto& exit : u->edges)
        {
            int alt = nodeDistance[u] + 1;

            if (alt < nodeDistance[exit.toNode])
            {
                nodeDistance[exit.toNode] = alt;
                nodePrevNode[exit.toNode] = u;
                nodePrevExit[exit.toNode] = &exit;
            }
        }
    }

    const Node* dest = &nodeB;
    QVector<const MapsGraph::Edge*> path;

    while (nodePrevNode[dest])
    {
        path.push_front(nodePrevExit[dest]);
        dest = nodePrevNode[dest];
    }


#ifdef DEBUG_MAPS_GRAPH_GENERATION
    if (nodePrevNode[&nodeB])
    {
        GLOBAL_INFO(QObject::tr(" >> Percorso da %1:%2 a %3:%4 ->")
                    .arg(nodeA.map->name()).arg(nodeA.group)
                    .arg(nodeB.map->name()).arg(nodeB.group));


        for (auto elem : path)
            GLOBAL_INFO(QObject::tr("     >> Uscita %1 da [%2:%3] a [%4:%5]")
                        .arg(RoomExit::roomExitToReadableString(elem->exit))
                        .arg(elem->room->name()).arg(elem->room->staticId())
                        .arg(elem->exitRoom->name()).arg(elem->exitRoom->staticId()));
    }
    else
        GLOBAL_INFO(QObject::tr(" >> Percorso da %1:%2 a %3:%4 -> Nessuno")
                    .arg(nodeA.map->name()).arg(nodeA.group)
                    .arg(nodeB.map->name()).arg(nodeB.group));
#endif

    return path;
}
