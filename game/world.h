/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef WORLD_H
#define WORLD_H

#include "playerconnection.h"
#include "player.h"
#include "character.h"
#include "pccharacter.h"
#include "map.h"
#include "statistics.h"
#include "mapsgraph.h"

#include <QObject>
#include <QVector>
#include <QMap>
#include <QDir>
#include <QTimer>
#include <QQmlEngine>


namespace NET
{
    class NetConnection;
}

namespace CORE
{

class World : public Entity
{
    Q_OBJECT

public:
    explicit World();
    virtual ~World() Q_DECL_OVERRIDE;

    bool init(QDir rootDir);
    Player *player(QMUD::IdType id) const;
    QMap<QMUD::IdType, Player*> playersByConnectionId() const;

    QString name(Entity *suitor = Q_NULLPTR) const Q_DECL_OVERRIDE;

    void flushPlayers();

    Map *map(QMUD::StaticIdType id) const;
    QMap<QMUD::StaticIdType, Map*> maps() const;
    QMap<QMUD::IdType, Item*> items() const;

    const MapsGraph& mapsGraph() const;

    Map *loadMap(QString filename);
    void removeMap(QMUD::StaticIdType id);

    Map *loadInstance(QString filename);
    void unloadEmptyInstances();

    void addGroup(Group* group);

    Character* character(QMUD::IdType id) const;
    QMap<QMUD::IdType, Character*> characters() const;

    void addItem(Item* item);

    int msNextTick() const;

    const QMap<QString, QMUD::PcCharacterBaseInfo>& pcCharactersBaseInfo() const;

    QMap<QMUD::StaticIdType, QMap<QMUD::StaticIdType, QMUD::Recipe>> recipes() const;
    const QMUD::Recipe& recipe(QMUD::StaticIdType mapId, QMUD::StaticIdType id) const;

    const GlobalStatistics& statistics() const;
    GlobalStatistics& statisticsFromLastReboot();

    QSet<QString> forbiddenPcNames() const;
    bool addForbiddenName(QString name);
    bool removeForbiddenName(QString name);

    const Quest *quest(const Quest::QuestIdType& id) const;

signals:

public slots:
    void onTimerUpdatePlayersTimeout();
    void onTimerUpdateCharactersTimeout();
    void onTimerUpdateCharactersStatusTimeout();
    void onTimerUpdateNpcResurrectionTimeout();
    void onTimerUpdateEntityScriptEvent();
    void onTimerUpdateMapsTimeout();
    void onTimerUpdateItems();
    void onTimerUpdateWorld();
    void onConnection(NET::NetConnection *conn);
    void onDisconnected(PlayerConnection *conn, Player* player);
    void onCommandReceived(PlayerConnection *conn, Player* player, QString cmd);
    void onNpcCommandReceived(NpcCharacter* npc, QString cmd);
    void onRemoveFromTheWorld(Character* ch);
    void onRemoveFromTheWorld(Item* item);
    void onEntityInsertEventCustom(QMUD::IdType id, int seconds, QString event);
    void onGroupDisbanded(Group*group);

private:
    void checkRoomExit(Map* p_map);
    bool createMapsTree();
    bool checkPlayerAndCharacterName(QString name);
    void removeConnection(QMUD::IdType id);
    void removeCharacter(QMUD::IdType id);
    void removeItem(QMUD::IdType id);
    void addPlayer(Player* player);
    void addCharacter(Character* ch);
    void deleteMap(Map* map);
    bool loadForbiddenPcNames(QString filePath);
    bool saveForbiddenPcNames(QString filePath);
    bool initQuests();

private:
    QTimer m_timerUpdatePlayers;
    QTimer m_timerUpdateCharacters;
    QTimer m_timerUpdateCharactersStatus;
    QTimer m_timerUpdateNpcResurrection;
    QTimer m_timerUpdateEntityScriptEvent;
    QTimer m_timerUpdateMaps;
    QTimer m_timerUpdateItems;
    QTimer m_timerUpdateWorld;

    QDir m_dirRoot;
    QDir m_dirPlayers;
    QDir m_dirCharacters;
    QDir m_dirMaps;

    QSet<QString> m_setForbiddenPcNames;

    QMap<QString, QMUD::PcCharacterBaseInfo> m_mapPcCharactersBaseInfo;
    QMap<QMUD::IdType, PlayerConnection*> m_mapConnections;
    QMap<QMUD::IdType, Player*> m_mapPlayersByConnectionId;
    QMap<QMUD::IdType, Character*> m_mapCharacters;
    QMap<QMUD::IdType, Item*> m_mapItems;
    QMap<QMUD::StaticIdType, Map*> m_mapMaps;
    QMap<Quest::QuestIdType, Quest> m_mapQuestsById; // < < map id, quest id >, quest >
    QMap<QPair<QMUD::StaticIdType, QMUD::StaticIdType>, QVector<Quest*>> m_mapQuestsByNpcStaticId; // < < map static id, npc static id >, quest vector >
    QSet<Group*> m_vGroups;

    MapsGraph m_mapsGraph;

    QMUD::StaticIdType m_iInstanceStaticIdCurrent;

    struct NpcToResurrectData
    {
        NpcToResurrectData() :
            resurrectionCounter(QMUD_GLOBAL_NPC_RESURRECTION_COUNTER),
            id(QMUD::StaticIdInvalid),
            mapId(QMUD::StaticIdInvalid),
            roomId(QMUD::StaticIdInvalid)
        {}

        int resurrectionCounter;
        QMUD::StaticIdType id;
        QMUD::StaticIdType mapId;
        QMUD::StaticIdType roomId;
    };

    QVector<NpcToResurrectData> m_vNpcToResurrect;

    // Script
    QQmlEngine m_jsEngine;

    struct ScriptEventCustomData
    {
        ScriptEventCustomData() :
            entity(QMUD::IdInvalid)
        {}

        int counter;
        QMUD::IdType entity;
        QString event;
    };

    QVector<ScriptEventCustomData> m_vScriptEvent;

    // Recipe
    QMap<QMUD::StaticIdType, QMap<QMUD::StaticIdType, QMUD::Recipe>> m_mapRecipes;

    // Statistiche
    GlobalStatistics m_globalStatistics;
    GlobalStatistics m_globalStatisticsFromLastReboot;
};

} // namespace CORE

#endif // WORLD_H
