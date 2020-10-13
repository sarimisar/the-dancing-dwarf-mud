/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef MAP_H
#define MAP_H

#include "entity.h"
#include "quest.h"
#include "../global.h"

#include <QMap>
#include <QDir>
#include <QDateTime>

namespace CORE
{

class Room;
class Character;
class PcCharacter;
class NpcCharacter;
class Item;

class Map : public Entity
{
    Q_OBJECT

public:
    virtual ~Map() Q_DECL_OVERRIDE;

    static Map* loadMap(QString fileName,
                        QMUD::FileOpStatus &status,
                        Entity* parent,
                        QVector<CORE::NpcCharacter *> &npcs,
                        QVector<CORE::Item *> &items,
                        QVector<QMUD::Recipe> &recipes,
                        QVector<Quest> &quests,
                        QMUD::StaticIdType instanceStaticId = QMUD::StaticIdInvalid);

    // Id della mappa
    QMUD::StaticIdType staticId() const;

    // Id originale della mappa
    // In generale e' uguale a staticId tranne nel caso
    // delle istanze
    QMUD::StaticIdType originalStaticId() const;

    // Path del file contenente la definizione della mappa
    QString filePath() const;

    // Gestione gruppi di stanze
    int groupCount() const;
    void setGroupCount(int count);

    // Update
    void update();

    // Stanze
    Room* room(QMUD::StaticIdType staticId);
    Room* room(int x, int y, QMUD::StaticIdType mapIndex);
    QMap<QMUD::StaticIdType, Room*> rooms() const;

    // Informazioni
    QString name(Entity *suitor = Q_NULLPTR) const Q_DECL_OVERRIDE;
    bool isInstance() const;

    QMap<QMUD::StaticIdType, QJsonObject> mapJsonObjNPCsById() const;
    QMap<QMUD::StaticIdType, QJsonObject> mapJsonObjItemsById() const;

    // NPC room
    QVector<Room *> npcRoomByStaticId(QMUD::StaticIdType npcStaticId) const;

    // Gestione Character
    QVector<Character*> characters() const;
    Character* character(QMUD::IdType id) const;
    void addCharacter(Character* ch);
    bool removeCharacter(Character* ch);

    // Istanza
    void instanceBindPcCharacter(QString name);
    void instanceUnbindPcCharacter(QString name);
    bool instancePcCharacterIsBind(QString name) const;
    QString instanceEntryPoint() const;
    QDateTime instanceCreationTime() const;
    QDateTime instanceLastEntryTime() const;

    // Wilderness
    int width(QMUD::StaticIdType mapIndex) const;
    int height(QMUD::StaticIdType mapIndex) const;
    int wildMapsCount() const;
    QList<QMUD::StaticIdType> wildMapsIndexes() const;
    QString wildMapName(QMUD::StaticIdType index) const;

protected:
    explicit Map(Entity *parent = Q_NULLPTR);

    bool generateRooms(QMap<QMUD::StaticIdType, QVector<QVector<int> > > mapGroups);

    bool loadMaps(const QJsonObject &obj, QMUD::FileOpStatus &status, QMap<QMUD::StaticIdType, QVector<QVector<int> > > &mapGroups);
    bool loadRooms(const QJsonObject &obj, QMUD::FileOpStatus &status, QVector<NpcCharacter *>& npcs, QVector<CORE::Item *> &items);
    bool loadResources(const QJsonObject &obj, QMUD::FileOpStatus &status, QVector<CORE::Item *> &items);
    bool loadRecipes(const QJsonObject &obj, QMUD::FileOpStatus &status, QVector<QMUD::Recipe> &recipes);
    bool loadQuests(const QJsonObject &obj, QMUD::FileOpStatus &status, QVector<Quest> &quests);

private:
    QString m_strFilePath;
    QMUD::StaticIdType m_iStaticId;
    QMUD::StaticIdType m_iOriginalStaticId;

    QString m_strName;

    QMap<QMUD::StaticIdType, QJsonObject> m_mapJsonObjNPCsById;
    QMap<QMUD::StaticIdType, QJsonObject> m_mapJsonObjItemsById;

    QMap<QMUD::StaticIdType, QVector<Room*>> m_mapNpcsRooms;

    int m_iGroupCount;

    // Characters
    QVector<Character*> m_vCharacters;

    // Istanza
    QSet<QString> m_vInstaneBindPcCharacterName;
    QString m_strInstanceEntryPoint;
    QDateTime m_instanceCreationTime;
    QDateTime m_instanceLastEntry;

    // Wild
    struct MapData
    {
        QMUD::StaticIdType index;
        QString name;
        QVector<QVector<bool>> isRoom;
        QVector<QVector<QVector<QMUD::RoomFlagType>>> flags;
    };

    QMap<QMUD::StaticIdType, MapData> m_mapMapDataByIndex;
    QMap<QMUD::StaticIdType, Room*> m_mapRooms;
};

}

#endif // MAP_H
