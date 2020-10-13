/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "map.h"
#include "npccharacter.h"
#include "item.h"
#include "itemresource.h"
#include "room.h"
#include "../globaldebug.h"
#include "connectedcomponentalgorithm.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSet>
#include <QImage>

using namespace CORE;

Map::Map(Entity *parent) :
    Entity(EntityType::MAP, parent),
    m_iStaticId(QMUD::StaticIdInvalid),
    m_iOriginalStaticId(QMUD::StaticIdInvalid),
    m_iGroupCount(0),
    m_instanceLastEntry(QDateTime::currentDateTime())
{

}

Map::~Map()
{

}

Map *Map::loadMap(QString fileName,
                  QMUD::FileOpStatus &status,
                  Entity *parent, QVector<NpcCharacter *>& npcs,
                  QVector<CORE::Item *> &items,
                  QVector<QMUD::Recipe> &recipes,
                  QVector<Quest> &quests,
                  QMUD::StaticIdType instanceStaticId)
{
    status = QMUD::FileOpStatus::FILE_OP_ERROR_GENERIC;

    // Apro il file json della mappa
    QString mapFilePath = fileName;

    if (!QFileInfo(mapFilePath).exists())
    {
        status = QMUD::FileOpStatus::FILE_OP_ERROR_INVALID_NAME;
        return Q_NULLPTR;
    }

    QFile fp(mapFilePath);
    if (!fp.open(QFile::ReadOnly | QFile::Text))
    {
        GLOBAL_CRITICAL(tr("Errore nell'apertura del file %1").arg(mapFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_OPEN_FILE;
        return Q_NULLPTR;
    }

    // Parso il file json
    QByteArray content = fp.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(content, &error);
    fp.close();

    if (error.error != QJsonParseError::NoError)
    {
        int lineNumber = 0;
        for (int i=0; i< error.offset; i++)
            if (content[i] == '\n')
                lineNumber++;

        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: %2 (linea %3)")
                        .arg(mapFilePath)
                        .arg(error.errorString())
                        .arg(lineNumber));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return Q_NULLPTR;
    }

    // Controllo la presenza della chiave id
    QJsonObject obj = doc.object();

    auto itId = obj.find("id");

    if (itId == obj.end())
    {
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'id' non presente")
                        .arg(mapFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return Q_NULLPTR;
    }

    QMUD::StaticIdType mapStaticId = static_cast<QMUD::StaticIdType>(itId.value().toInt());
    QMUD::StaticIdType originalMapStaticId = mapStaticId;

    if (instanceStaticId != QMUD::StaticIdInvalid)
        mapStaticId = instanceStaticId;

    // Controllo la presenza della chiave name
    auto itName = obj.find("name");

    if (itName == obj.end())
    {
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'name' non presente")
                        .arg(mapFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return Q_NULLPTR;
    }

    // Creo la mappa
    Map *map = new Map();
    map->setParent(parent);
    map->m_strFilePath = mapFilePath;
    map->m_iStaticId = mapStaticId;
    map->m_iOriginalStaticId = originalMapStaticId;
    map->m_strName = itName.value().toString();
    map->m_strInstanceEntryPoint = QFileInfo(fileName).baseName();
    map->m_instanceCreationTime = QDateTime::currentDateTime();

    // Carico la mappa ascii
    QVector<QVector<int> > mapGroups;

    // Carico le stanze
    if (!map->loadRooms(obj, status, npcs, items))
    {
        delete map;
        return Q_NULLPTR;
    }

    // Posiziono le risorse
    if (!map->loadResources(obj, status, items))
    {
        delete map;
        return Q_NULLPTR;
    }

    // Carico la lista delle recipe
    if (!map->loadRecipes(obj, status, recipes))
    {
        delete map;
        return Q_NULLPTR;
    }

    // Carico la lista delle quest
    if (!map->loadQuests(obj, status, quests))
    {
        delete map;
        return Q_NULLPTR;
    }

    status = QMUD::FileOpStatus::FILE_OP_OK;
    return map;
}

QMUD::StaticIdType Map::staticId() const
{
    return m_iStaticId;
}

QMUD::StaticIdType Map::originalStaticId() const
{
    return m_iOriginalStaticId;
}

QString Map::filePath() const
{
    return m_strFilePath;
}

int Map::groupCount() const
{
    return m_iGroupCount;
}

void Map::setGroupCount(int count)
{
    m_iGroupCount = count;
}

void Map::update()
{

}

Room *Map::room(QMUD::StaticIdType staticId)
{
    auto it = m_mapRooms.find(staticId);

    if (it == m_mapRooms.end())
        return Q_NULLPTR;
    else
        return it.value();
}

Room *Map::room(int x, int y, QMUD::StaticIdType mapIndex)
{
    auto mapDataIt = m_mapMapDataByIndex.find(mapIndex);

    if (mapDataIt == m_mapMapDataByIndex.end())
        return Q_NULLPTR;

    Q_ASSERT(!mapDataIt->isRoom.isEmpty());

    if (y < 0 || y >= mapDataIt->isRoom.size() || x < 0 || x >= mapDataIt->isRoom[0].size())
        return Q_NULLPTR;
    else
        return room(static_cast<QMUD::StaticIdType>(y * mapDataIt->isRoom[0].size() + x) + QMUD_WILD_ROOM_BASE_INDEX);
}

QString Map::name(Entity *suitor) const
{
    Q_UNUSED(suitor)

    return m_strName;
}

QMap<QMUD::StaticIdType, QJsonObject> Map::mapJsonObjNPCsById() const
{
    return m_mapJsonObjNPCsById;
}

QMap<QMUD::StaticIdType, QJsonObject> Map::mapJsonObjItemsById() const
{
    return m_mapJsonObjItemsById;
}

QVector<Room*> Map::npcRoomByStaticId(QMUD::StaticIdType npcStaticId) const
{
    auto rooms = m_mapNpcsRooms.find(npcStaticId);

    if (rooms == m_mapNpcsRooms.end())
        return QVector<Room*>();

    return rooms.value();
}

bool Map::isInstance() const
{
    return m_iStaticId >= QMUD_INSTANCE_FIRST_STATIC_ID;
}

QVector<Character *> Map::characters() const
{
    return m_vCharacters;
}

Character *Map::character(QMUD::IdType id) const
{
    for (auto ch : m_vCharacters)
        if (ch->id() == id)
            return ch;

    return Q_NULLPTR;
}

void Map::addCharacter(Character *ch)
{
    if (m_vCharacters.contains(ch))
        return;

    m_vCharacters.push_back(ch);

    if (isInstance() && ch->chType() == Character::ChType::CHTYPE_PC)
        m_instanceLastEntry = QDateTime::currentDateTime();
}

bool Map::removeCharacter(Character *ch)
{
    bool ret = m_vCharacters.removeOne(ch);

    if (ret && isInstance() && ch->chType() == Character::ChType::CHTYPE_PC)
        m_instanceLastEntry = QDateTime::currentDateTime();

    return ret;
}

void Map::instanceBindPcCharacter(QString name)
{
    if (isInstance())
    {
        m_vInstaneBindPcCharacterName.insert(name);
        m_instanceLastEntry = QDateTime::currentDateTime();
    }
}

void Map::instanceUnbindPcCharacter(QString name)
{
    if (isInstance())
        m_vInstaneBindPcCharacterName.remove(name);
}

bool Map::instancePcCharacterIsBind(QString name) const
{
    return isInstance() && m_vInstaneBindPcCharacterName.contains(name);
}

QString Map::instanceEntryPoint() const
{
    return m_strInstanceEntryPoint;
}

QDateTime Map::instanceCreationTime() const
{
    return m_instanceCreationTime;
}

QDateTime Map::instanceLastEntryTime() const
{
    return m_instanceLastEntry;
}

QMap<QMUD::StaticIdType, Room*> Map::rooms() const
{
    return m_mapRooms;
}

int Map::width(QMUD::StaticIdType mapIndex) const
{
    auto mapDataIt = m_mapMapDataByIndex.find(mapIndex);

    if (mapDataIt == m_mapMapDataByIndex.end())
        return 0;

    if (mapDataIt->isRoom.isEmpty())
        return 0;
    else
        return mapDataIt->isRoom[0].size();
}

int Map::height(QMUD::StaticIdType mapIndex) const
{
    auto mapDataIt = m_mapMapDataByIndex.find(mapIndex);

    if (mapDataIt == m_mapMapDataByIndex.end())
        return 0;

    return mapDataIt->isRoom.size();
}

int Map::wildMapsCount() const
{
    return m_mapMapDataByIndex.size();
}

QList<QMUD::StaticIdType> Map::wildMapsIndexes() const
{
    return m_mapMapDataByIndex.keys();
}

QString Map::wildMapName(QMUD::StaticIdType mapIndex) const
{
    auto mapDataIt = m_mapMapDataByIndex.find(mapIndex);

    if (mapDataIt == m_mapMapDataByIndex.end())
        return QString();

    return mapDataIt->name;
}

bool Map::generateRooms(QMap<QMUD::StaticIdType, QVector<QVector<int>>> mapGroups)
{
    if (m_mapMapDataByIndex.isEmpty())
        return true;

    for (auto mapDataIt = m_mapMapDataByIndex.begin(); mapDataIt != m_mapMapDataByIndex.end(); ++mapDataIt)
    {
        if (mapDataIt->isRoom.isEmpty())
            continue;

        int mapWidth = mapDataIt->isRoom[0].size();
        int mapHeight = mapDataIt->isRoom.size();

        QMUD::StaticIdType mapBaseStaticId = QMUD_WILD_ROOM_BASE_INDEX + static_cast<QMUD::StaticIdType>(mapDataIt->index) * QMUD_WILD_ROOM_RESERVE_INDEX_FOR_MAP;

        for (QMUD::StaticIdType staticId = 0; staticId < static_cast<unsigned int>(mapWidth * mapHeight); ++staticId)
        {
            int row = static_cast<int>(staticId / static_cast<unsigned int>(mapWidth));
            int column = static_cast<int>(staticId - static_cast<unsigned int>(row * mapWidth));

            Room *room = Q_NULLPTR;

            auto it = m_mapRooms.find(staticId + mapBaseStaticId);

            if (it == m_mapRooms.end())
            {
                if (!mapDataIt->isRoom[row][column])
                    continue;

                // Creo la stanza
                room = new Room();
                room->setParent(this);
                room->m_iStaticId = staticId + mapBaseStaticId;
            }
            else
            {
                room = it.value();
            }

            if (room->m_strDescription.isEmpty())
            {
                room->m_bIsWildDescription = true;
                room->m_iWildY = row;
                room->m_iWildX = column;
                room->m_iWildGroup = mapGroups[mapDataIt->index][row][column];
                room->m_iWildMapIndex = mapDataIt->index;
                room->m_strName = "";
                room->m_vFlags = mapDataIt->flags[row][column];

                // Aggiungo le uscite
                int tmpRow = row - 1;
                int tmpColumn = column;

                if (!room->m_exitNorth.isValid() && !(tmpRow < 0 || tmpRow >= mapHeight || tmpColumn < 0 || tmpColumn >= mapWidth) && mapDataIt->isRoom[tmpRow][tmpColumn])
                {
                    room->m_exitNorth.m_bIsValid = true;
                    room->m_exitNorth.m_iMapStaticId = m_iStaticId;
                    room->m_exitNorth.m_iRoomStaticId = static_cast<QMUD::StaticIdType>(tmpRow * mapWidth + tmpColumn) + mapBaseStaticId;
                    room->m_exitNorth.m_eExitType = RoomExit::RoomExitType::EXIT_NORTH;
                }

                tmpRow = row + 1;
                tmpColumn = column;

                if (!room->m_exitSouth.isValid() && !(tmpRow < 0 || tmpRow >= mapHeight || tmpColumn < 0 || tmpColumn >= mapWidth) && mapDataIt->isRoom[tmpRow][tmpColumn])
                {
                    room->m_exitSouth.m_bIsValid = true;
                    room->m_exitSouth.m_iMapStaticId = m_iStaticId;
                    room->m_exitSouth.m_iRoomStaticId = static_cast<QMUD::StaticIdType>(tmpRow * mapWidth + tmpColumn) + mapBaseStaticId;
                    room->m_exitSouth.m_eExitType = RoomExit::RoomExitType::EXIT_SOUTH;
                }

                tmpRow = row;
                tmpColumn = column - 1;

                if (!room->m_exitWest.isValid() && !(tmpRow < 0 || tmpRow >= mapHeight || tmpColumn < 0 || tmpColumn >= mapWidth) && mapDataIt->isRoom[tmpRow][tmpColumn])
                {
                    room->m_exitWest.m_bIsValid = true;
                    room->m_exitWest.m_iMapStaticId = m_iStaticId;
                    room->m_exitWest.m_iRoomStaticId = static_cast<QMUD::StaticIdType>(tmpRow * mapWidth + tmpColumn) + mapBaseStaticId;
                    room->m_exitWest.m_eExitType = RoomExit::RoomExitType::EXIT_WEST;
                }

                tmpRow = row;
                tmpColumn = column + 1;

                if (!room->m_exitEast.isValid() && !(tmpRow < 0 || tmpRow >= mapHeight || tmpColumn < 0 || tmpColumn >= mapWidth) && mapDataIt->isRoom[tmpRow][tmpColumn])
                {
                    room->m_exitEast.m_bIsValid = true;
                    room->m_exitEast.m_iMapStaticId = m_iStaticId;
                    room->m_exitEast.m_iRoomStaticId = static_cast<QMUD::StaticIdType>(tmpRow * mapWidth + tmpColumn) + mapBaseStaticId;
                    room->m_exitEast.m_eExitType = RoomExit::RoomExitType::EXIT_EAST;
                }

                room->updateExitsMap();
            }

            m_mapRooms[staticId + mapBaseStaticId] = room;
        }
    }

    return true;
}

bool Map::loadMaps(const QJsonObject &obj, QMUD::FileOpStatus &status, QMap<QMUD::StaticIdType, QVector<QVector<int>>> &mapGroups)
{
    auto itMaps = obj.find("maps");

    if (itMaps == obj.end())
        return true;

    if (!itMaps->isArray())
    {
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento 'maps' non valido").arg(m_strFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return false;
    }

    auto mapsArray = itMaps->toArray();

    for (auto itMap = mapsArray.begin(); itMap != mapsArray.end(); ++itMap)
    {
        if (!itMap->isObject())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento 'maps' non valido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        auto objMap = itMap->toObject();

        auto itIndex = objMap.find("index");
        auto itName = objMap.find("name");
        auto itRooms = objMap.find("rooms");

        if (itIndex == objMap.end() || !itIndex->isDouble())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento 'maps->index' non presente o non valido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        if (itName == objMap.end() || !itName->isString())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento 'maps->name' non presente o non valido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        if (itRooms == objMap.end() || !itRooms->isArray())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento 'maps->rooms' non presente o non valido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        QJsonArray roomsArray = itRooms->toArray();

        QVector<QVector<bool>> valuesRooms;
        valuesRooms.resize(roomsArray.size());

        int row = 0;

        for (auto itRoom = roomsArray.begin(); itRoom != roomsArray.end(); ++itRoom, ++row)
        {
            if (!itRoom->isString())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento 'maps->rooms' non valido").arg(m_strFilePath));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            QString str = itRoom->toString();

            if (row != 0 && str.size() != valuesRooms[0].size())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento 'maps->rooms' non valido").arg(m_strFilePath));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            valuesRooms[row].resize(str.size());
            for (int i = 0; i < str.size(); ++i)
                    valuesRooms[row][i] = (str[i] == "1");
        }

        QMUD::StaticIdType index = static_cast<QMUD::StaticIdType>(qRound64(itIndex->toDouble()));
        QString name = itName->toString();

        MapData mapData;
        mapData.name = name;
        mapData.index = index;
        mapData.isRoom = valuesRooms;

        mapData.flags.resize(valuesRooms.size());
        for (auto& elem : mapData.flags)
            elem.resize(valuesRooms[0].size());

        m_mapMapDataByIndex[index] = mapData;

        mapGroups[index].resize(valuesRooms.size());

        for (int x = 0; x < valuesRooms.size(); ++x)
        {
            mapGroups[index][x].resize(valuesRooms[x].size());

            for (int y = 0; y < valuesRooms[x].size(); ++y)
            {
                if (!valuesRooms[x][y])
                    mapGroups[index][x][y] = 0;
                else
                    mapGroups[index][x][y] = 1;
            }
        }

        ConnectedComponentAlgorithm ccAlg(mapGroups[index]);
        QVector<int> labels;
        ccAlg.getOut(mapGroups[index], labels);
    }

    return true;
}

bool Map::loadRooms(const QJsonObject &obj, QMUD::FileOpStatus &status, QVector<NpcCharacter *>& npcs, QVector<CORE::Item *> &items)
{
    QMap<QMUD::StaticIdType, QVector<QVector<int>>> mapGroups;

    if (!loadMaps(obj, status, mapGroups))
        return false;

    auto itRooms = obj.find("rooms");

    if (itRooms != obj.end() & !itRooms.value().isArray())
    {
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'rooms' non valido").arg(m_strFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return false;
    }

    QMap<QMUD::StaticIdType, QVector<QPair<QMUD::StaticIdType, bool>>> npcsByRoomId;
    QMap<QMUD::StaticIdType, QVector<QMUD::StaticIdType>> itemsByRoomId;

    if (itRooms != obj.end())
    {
        QJsonArray objRooms = itRooms.value().toArray();

        int elementIndex = 0;

        for (auto roomObj : objRooms)
        {
            if (!roomObj.isObject())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento %2 di 'rooms' non valido").arg(m_strFilePath).arg(elementIndex));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            QVector<QPair<QMUD::StaticIdType, bool>> npcsInRoom;
            QVector<QMUD::StaticIdType> itemsInRoom;

            Room *room = Room::loadRoom(roomObj.toObject(), this, npcsInRoom, itemsInRoom);

            if (!room)
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: elemento %2 di 'rooms' non valido").arg(m_strFilePath).arg(elementIndex));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            auto itRoom = m_mapRooms.find(room->staticId());

            if (itRoom != m_mapRooms.end())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: staticId %2 dell'elemento %3 di 'rooms' non valido").arg(m_strFilePath).arg(room->staticId()).arg(elementIndex));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            m_mapRooms[room->staticId()] = room;
            npcsByRoomId[room->staticId()] = npcsInRoom;
            itemsByRoomId[room->staticId()] = itemsInRoom;

            elementIndex++;
        }
    }

    if (!generateRooms(mapGroups))
    {
        GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: Errore nella generazione delle stanze").arg(m_strFilePath));
        status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
        return false;
    }

    // Carico la lista degli Items
    auto itItems = obj.find("items");

    if (itItems != obj.end())
    {
        if (!itItems->isArray())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'items' invalido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        QJsonArray jsonArrayItems = itItems->toArray();

        for (int i = 0; i < jsonArrayItems.size(); ++i)
        {
            if (!jsonArrayItems[i].isObject())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'item' numero %2 invalido").arg(m_strFilePath).arg(i));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            QJsonObject jsonObjItem = jsonArrayItems[i].toObject();

            auto itItemId = jsonObjItem.find("id");

            if (itItemId == jsonObjItem.end() || !itItemId->isDouble())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'item' numero %2 ha id non valido").arg(m_strFilePath).arg(i));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            m_mapJsonObjItemsById[static_cast<QMUD::StaticIdType>(itItemId->toInt())] = jsonObjItem;
        }
    }

    // Carico la lista degli NPC
    auto itNpcs = obj.find("npcs");

    if (itNpcs != obj.end())
    {
        if (!itNpcs->isArray())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'npcs' invalido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        QJsonArray jsonArrayNPCs = itNpcs->toArray();

        for (int i = 0; i < jsonArrayNPCs.size(); ++i)
        {
            if (!jsonArrayNPCs[i].isObject())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'npc' numero %2 invalido").arg(m_strFilePath).arg(i));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            QJsonObject jsonObjNPC = jsonArrayNPCs[i].toObject();

            auto itNPCId = jsonObjNPC.find("id");

            if (itNPCId == jsonObjNPC.end() || !itNPCId->isDouble())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'npc' numero %2 ha id non valido").arg(m_strFilePath).arg(i));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            m_mapJsonObjNPCsById[static_cast<QMUD::StaticIdType>(itNPCId->toInt())] = jsonObjNPC;
        }
    }

    // Posiziono gli NPC
    for (auto itNpcsInRoom = npcsByRoomId.begin(); itNpcsInRoom != npcsByRoomId.end(); ++itNpcsInRoom)
    {
        Room* room = this->room(itNpcsInRoom.key());

        Q_ASSERT(room);

        for (auto npcInfo : itNpcsInRoom.value())
        {
            auto itNpcJsonObj = m_mapJsonObjNPCsById.find(npcInfo.first);

            if (itNpcJsonObj == m_mapJsonObjNPCsById.end())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: posizionamento npc %1 non valido").arg(m_strFilePath).arg(npcInfo.first));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            NpcCharacter* npcCh = NpcCharacter::loadNpcCharacter(itNpcJsonObj.value(), m_mapJsonObjItemsById, m_iStaticId, room);

            if (!npcCh)
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: posizionamento npc %1 non valido").arg(m_strFilePath).arg(npcInfo.first));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            auto itNpcRooms = m_mapNpcsRooms.find(npcCh->staticId());

            if (itNpcRooms == m_mapNpcsRooms.end() || !itNpcRooms->contains(room))
            {
                if (itNpcRooms == m_mapNpcsRooms.end())
                    m_mapNpcsRooms[npcCh->staticId()].push_back(room);
                else
                    itNpcRooms->push_back(room);
            }


            npcCh->setToResurrect(npcInfo.second);
            npcs.push_back(npcCh);

            if (npcCh->flagCheck(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_SMALL) && room->isWilderness())
            {
                int wildX = room->wildX();
                int wildY = room->wildY();

                Room* newRoom = room;
                int count = 0;

                while (count < 10)
                {
                    wildX += QMUD::rollDice(1, QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL * 2 + 1) - QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL;
                    wildY += QMUD::rollDice(1, QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL * 2 + 1) - QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL;

                    auto randomRoom = this->room(wildX, wildY, room->wildMapIndex());

                    if (randomRoom)
                    {
                        newRoom = randomRoom;
                        break;
                    }

                    count++;
                }

                npcCh->setParent(newRoom);
            }
            else if (npcCh->flagCheck(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_LARGE) && room->isWilderness())
            {
                int wildX = room->wildX();
                int wildY = room->wildY();

                Room* newRoom = room;
                int count = 0;

                while (count < 10)
                {
                    wildX += QMUD::rollDice(1, QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE * 2 + 1) - QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE;
                    wildY += QMUD::rollDice(1, QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE * 2 + 1) - QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE;

                    auto randomRoom = this->room(wildX, wildY, room->wildMapIndex());

                    if (randomRoom)
                    {
                        newRoom = randomRoom;
                        break;
                    }

                    count++;
                }

                npcCh->setParent(newRoom);
            }
        }
    }

    // Posiziono gli Items
    for (auto itItemsInRoom = itemsByRoomId.begin(); itItemsInRoom != itemsByRoomId.end(); ++itItemsInRoom)
    {
        Room* room = this->room(itItemsInRoom.key());

        Q_ASSERT(room);

        for (auto itemId : itItemsInRoom.value())
        {
            auto itItemJsonObj = m_mapJsonObjItemsById.find(static_cast<QMUD::StaticIdType>(itemId));

            if (itItemJsonObj == m_mapJsonObjItemsById.end())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: posizionamento item %2 non valido").arg(m_strFilePath).arg(itemId));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            Item* item = Item::loadItem(itItemJsonObj.value(), room, static_cast<QMUD::StaticIdType>(m_iOriginalStaticId));

            if (!item)
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: posizionamento item %2 non valido").arg(m_strFilePath).arg(itemId));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            items.push_back(item);
        }
    }

    return true;
}

bool Map::loadResources(const QJsonObject &obj, QMUD::FileOpStatus &status, QVector<CORE::Item *> &items)
{
    auto itResources = obj.find("resources");

    if (itResources != obj.end())
    {
        if (!itResources->isArray())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'resources' invalido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        QJsonArray jsonArrayResources = itResources->toArray();

        for (int jsonResourceIndex = 0; jsonResourceIndex < jsonArrayResources.size(); ++jsonResourceIndex)
        {
            if (!jsonArrayResources[jsonResourceIndex].isObject())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'resources' numero %2 invalido").arg(m_strFilePath).arg(jsonResourceIndex));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            QJsonObject jsonObjResource = jsonArrayResources[jsonResourceIndex].toObject();

            auto itRarity = jsonObjResource.find("rarity");
            auto itResource = jsonObjResource.find("resource");

            if (itRarity == jsonObjResource.end() || !itRarity->isString() ||
                itResource == jsonObjResource.end() || !itResource->isString())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'resources' numero %2 invalido").arg(m_strFilePath).arg(jsonResourceIndex));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            QMUD::ResourceType resourceType = QMUD::resourceFromString(itResource->toString());

            if (resourceType == QMUD::ResourceType::UNKNOWN)
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'resources' numero %2 invalido, risorsa %3 non valida").arg(m_strFilePath).arg(jsonResourceIndex).arg(itResource->toString()));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            QMUD::ResourceRarityType rarityType = QMUD::resourceRarityFromString(itRarity->toString());

            if (rarityType == QMUD::ResourceRarityType::UNKNOWN)
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'resources' numero %2 invalido, risorsa %3 non valida").arg(m_strFilePath).arg(jsonResourceIndex).arg(itResource->toString()));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            int numberOfRooms = this->rooms().size();

            int availability = numberOfRooms;

            if (rarityType != QMUD::ResourceRarityType::NONE)
            {
                if (rarityType == QMUD::ResourceRarityType::NORMAL)
                    availability = qMin(200, numberOfRooms);
                if (rarityType == QMUD::ResourceRarityType::UNCOMMON)
                    availability = qMin(400, numberOfRooms);
                else if (rarityType == QMUD::ResourceRarityType::RARE)
                    availability = qMin(700, numberOfRooms);
                else if (rarityType == QMUD::ResourceRarityType::VERY_RARE)
                    availability = qMin(2000, numberOfRooms);
                else if (rarityType == QMUD::ResourceRarityType::EXTREMELY_RARE)
                    availability = numberOfRooms;

                int numberOfResources = numberOfRooms / availability;

                for (int resourceIndex = 0; resourceIndex < numberOfResources; ++resourceIndex)
                {
                    int tryCounter = 0;

                    while (true)
                    {
                        auto index = wildMapsIndexes()[QMUD::rollDice(1, wildMapsCount()) - 1];
                        int x = QMUD::rollDice(1, width(index)) - 1;
                        int y = QMUD::rollDice(1, height(index)) - 1;

                        Room *room = this->room(x, y, index);

                        if (room && room->canContainResources())
                        {
                            Item* item = ItemResource::create(resourceType, room);

                            if (!item)
                            {
                                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'resources' numero %2 invalido, errore nella creazione della risorsa %3").arg(m_strFilePath).arg(resourceIndex).arg(itResource->toString()));
                                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                                return false;
                            }

                            items.push_back(item);
                            break;
                        }
                        else
                            tryCounter++;

                        if (tryCounter > 10)
                            break;
                    }
                }
            }
        }
    }

    return true;
}

bool Map::loadRecipes(const QJsonObject &obj, QMUD::FileOpStatus &status, QVector<QMUD::Recipe> &recipes)
{
    auto itRecipes = obj.find("recipes");

    if (itRecipes != obj.end())
    {
        if (!itRecipes->isArray())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'recipes' non valido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        auto arrayRecipes = itRecipes.value().toArray();

        for (auto val : arrayRecipes)
        {
            if (!val.isObject())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'recipes' non valido (recipe non valida)").arg(m_strFilePath));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            QMUD::Recipe recipe;

            if (!recipe.read(val.toObject(), m_iStaticId))
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'recipes' non valido (recipe non valida)").arg(m_strFilePath));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            recipes.push_back(recipe);
        }
    }

    return true;
}

bool Map::loadQuests(const QJsonObject &obj, QMUD::FileOpStatus &status, QVector<Quest> &quests)
{
    auto itQuests = obj.find("quests");

    if (itQuests != obj.end())
    {
        if (!itQuests->isArray())
        {
            GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'quests' non valido").arg(m_strFilePath));
            status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
            return false;
        }

        auto arrayQuests = itQuests.value().toArray();

        for (auto val : arrayQuests)
        {
            if (!val.isObject())
            {
                GLOBAL_CRITICAL(tr("Errore nel parsing del file %1: 'quests' non valido (quest non valida)").arg(m_strFilePath));
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            Quest quest;
            if (!quest.read(val.toObject(), m_iOriginalStaticId))
            {
                status = QMUD::FileOpStatus::FILE_OP_ERROR_CORRUPTED_FILE;
                return false;
            }

            status = QMUD::FileOpStatus::FILE_OP_OK;
            quests.push_back(quest);
        }
    }

    return true;
}
