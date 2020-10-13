/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "room.h"
#include "character.h"
#include "npccharacter.h"
#include "map.h"
#include "itemresource.h"
#include "player.h"
#include "../global.h"
#include "../globaldebug.h"
#include "datetime.h"
#include "world.h"

#include <QJsonArray>
#include <QChildEvent>

using namespace CORE;


Room::Room(Map *parent) :
    Entity(EntityType::ROOM, parent),
    m_iStaticId(0),
    m_bIsWildDescription(false),
    m_iWildX(0),
    m_iWildY(0),
    m_iWildGroup(-1),
    m_iWildMapIndex(QMUD::StaticIdInvalid),
    m_iGroup(-1),
    m_iMark(QMUD::IdInvalid),
    m_iInstanceExitMapStaticId(QMUD::StaticIdInvalid),
    m_iInstanceExitRoomStaticId(QMUD::StaticIdInvalid),
    m_ptrInstanceExitRoom(Q_NULLPTR)
{

}

Room::~Room()
{

}

Room *Room::loadRoom(const QJsonObject &obj, Map *map, QVector<QPair<QMUD::StaticIdType, bool>>& npcs, QVector<QMUD::StaticIdType>& items)
{
    QMUD::StaticIdType tmpId = QMUD::StaticIdInvalid;

    // Id
    if (!QMUD::fileJsonReadStaticId(obj, "id", tmpId, true, QMUD::StaticIdInvalid))
        return Q_NULLPTR;

    auto coord = obj.find("roomCoord");

    if (coord != obj.end())
    {
        if (!coord->isArray())
        {
            GLOBAL_CRITICAL(tr("Chiave 'roomCoord' non valida"));
            return Q_NULLPTR;
        }

        auto coordArray = coord->toArray();

        if (coordArray.size() != 3 || !coordArray[0].isDouble() || !coordArray[1].isDouble() || !coordArray[2].isDouble())
        {
            GLOBAL_CRITICAL(tr("Chiave 'roomCoord' non valida"));
            return Q_NULLPTR;
        }

        if (coordArray[1].toInt() != -1 && coordArray[0].toInt() != -1)
        {
            QMUD::StaticIdType index = static_cast<QMUD::StaticIdType>(qRound64(coordArray[2].toDouble()));
            tmpId = static_cast<QMUD::StaticIdType>(map->width(index) * coordArray[1].toInt() + coordArray[0].toInt()) + QMUD_WILD_ROOM_BASE_INDEX + index * QMUD_WILD_ROOM_RESERVE_INDEX_FOR_MAP;

            if (tmpId < QMUD_WILD_ROOM_BASE_INDEX)
            {
                GLOBAL_CRITICAL(tr("Chiave 'roomCoord' non valida"));
                return Q_NULLPTR;
            }
        }
    }

    if (tmpId == QMUD::StaticIdInvalid)
    {
        GLOBAL_CRITICAL(tr("Chiavi 'id' e 'roomCoord' non presenti o non valide"));
        return Q_NULLPTR;
    }

    // Name
    QString tmpName;
    if (!QMUD::fileJsonRead(obj, "name", tmpName, true, true))
        return Q_NULLPTR;

    // Description
    QStringList tmpDescription;
    if (!QMUD::fileJsonRead(obj, "description", tmpDescription, false, false, true))
        return Q_NULLPTR;

    // Uscite
    auto itExitN = obj.find("exitN");
    auto itExitS = obj.find("exitS");
    auto itExitW = obj.find("exitW");
    auto itExitE = obj.find("exitE");
    auto itExitU = obj.find("exitU");
    auto itExitD = obj.find("exitD");

    if (itExitN != obj.end() && !itExitN.value().isObject())
    {
        GLOBAL_CRITICAL(tr("Chiave 'exitN' non valida"));
        return Q_NULLPTR;
    }

    if (itExitS != obj.end() && !itExitS.value().isObject())
    {
        GLOBAL_CRITICAL(tr("Chiave 'exitS' non valida"));
        return Q_NULLPTR;
    }

    if (itExitW != obj.end() && !itExitW.value().isObject())
    {
        GLOBAL_CRITICAL(tr("Chiave 'exitW' non valida"));
        return Q_NULLPTR;
    }

    if (itExitE != obj.end() && !itExitE.value().isObject())
    {
        GLOBAL_CRITICAL(tr("Chiave 'exitE' non valida"));
        return Q_NULLPTR;
    }

    if (itExitU != obj.end() && !itExitU.value().isObject())
    {
        GLOBAL_CRITICAL(tr("Chiave 'exitU' non valida"));
        return Q_NULLPTR;
    }

    if (itExitD != obj.end() && !itExitD.value().isObject())
    {
        GLOBAL_CRITICAL(tr("Chiave 'exitD' non valida"));
        return Q_NULLPTR;
    }

    RoomExit exitN;
    RoomExit exitS;
    RoomExit exitW;
    RoomExit exitE;
    RoomExit exitU;
    RoomExit exitD;

    if (itExitN != obj.end())
    {
        exitN = RoomExit::loadExit(itExitN.value().toObject(), RoomExit::RoomExitType::EXIT_NORTH);

        if (!exitN.isValid())
        {
            GLOBAL_CRITICAL(tr("Chiave 'exitN' non valida"));
            return Q_NULLPTR;
        }
    }

    if (itExitS != obj.end())
    {
        exitS = RoomExit::loadExit(itExitS.value().toObject(), RoomExit::RoomExitType::EXIT_SOUTH);

        if (!exitS.isValid())
        {
            GLOBAL_CRITICAL(tr("Chiave 'exitS' non valida"));
            return Q_NULLPTR;
        }
    }

    if (itExitW != obj.end())
    {
        exitW = RoomExit::loadExit(itExitW.value().toObject(), RoomExit::RoomExitType::EXIT_WEST);

        if (!exitW.isValid())
        {
            GLOBAL_CRITICAL(tr("Chiave 'exitW' non valida"));
            return Q_NULLPTR;
        }
    }

    if (itExitE != obj.end())
    {
        exitE = RoomExit::loadExit(itExitE.value().toObject(), RoomExit::RoomExitType::EXIT_EAST);

        if (!exitE.isValid())
        {
            GLOBAL_CRITICAL(tr("Chiave 'exitE' non valida"));
            return Q_NULLPTR;
        }
    }

    if (itExitU != obj.end())
    {
        exitU = RoomExit::loadExit(itExitU.value().toObject(), RoomExit::RoomExitType::EXIT_UP);

        if (!exitU.isValid())
        {
            GLOBAL_CRITICAL(tr("Chiave 'exitU' non valida"));
            return Q_NULLPTR;
        }
    }

    if (itExitD != obj.end())
    {
        exitD = RoomExit::loadExit(itExitD.value().toObject(), RoomExit::RoomExitType::EXIT_DOWN);

        if (!exitD.isValid())
        {
            GLOBAL_CRITICAL(tr("Chiave 'exitD' non valida"));
            return Q_NULLPTR;
        }
    }

    // Entrata ad una istanza
    auto itInstanceEntryPoint = obj.find("instanceEntryPoint");

    if (itInstanceEntryPoint != obj.end() && !itInstanceEntryPoint.value().isString())
    {
        GLOBAL_CRITICAL(tr("Chiave 'instanceEntryPoint' non valida"));
        return Q_NULLPTR;
    }

    QString instanceEntryPoint;

    if (itInstanceEntryPoint != obj.end())
        instanceEntryPoint = itInstanceEntryPoint.value().toString();

    // Uscita dall'istanza
    auto itInstanceExitPointMap = obj.find("instanceExitPointMap");
    auto itInstanceExitPointRoom = obj.find("instanceExitPointRoom");

    if (itInstanceExitPointMap != obj.end() && !itInstanceExitPointMap.value().isDouble())
    {
        GLOBAL_CRITICAL(tr("Chiave 'instanceExitPointMap' non valida"));
        return Q_NULLPTR;
    }

    if (itInstanceExitPointRoom != obj.end() && !itInstanceExitPointRoom.value().isDouble())
    {
        GLOBAL_CRITICAL(tr("Chiave 'itInstanceExitPointRoom' non valida"));
        return Q_NULLPTR;
    }

    QMUD::StaticIdType instanceExitPointMap = QMUD::StaticIdInvalid;
    QMUD::StaticIdType instanceExitPointRoom = QMUD::StaticIdInvalid;

    if (itInstanceExitPointRoom != obj.end() && itInstanceExitPointRoom != obj.end())
    {
        instanceExitPointMap = static_cast<QMUD::StaticIdType>(itInstanceExitPointMap.value().toDouble());
        instanceExitPointRoom = static_cast<QMUD::StaticIdType>(itInstanceExitPointRoom.value().toDouble());
    }

    // Flags
    QStringList tmpFlags;
    if (!QMUD::fileJsonRead(obj, "flags", tmpFlags, true, true, true, true))
        return Q_NULLPTR;

    QVector<QMUD::RoomFlagType> flags;

    for (auto strFlag : tmpFlags)
    {
        QMUD::RoomFlagType flag = QMUD::roomFlagFromString(strFlag);

        if (flag != QMUD::RoomFlagType::UNKNOWN &&
                !flags.contains(flag))
            flags.push_back(flag);
    }

    // NPC
    auto itNpcs = obj.find("npcs");
    if (itNpcs != obj.end())
    {
        if (!itNpcs->isArray())
        {
            GLOBAL_CRITICAL(tr("Chiave 'npcs' non valida"));
            return Q_NULLPTR;
        }

        QJsonArray jsonArrayNpcs = itNpcs->toArray();

        for (auto elem : jsonArrayNpcs)
        {
            if (!elem.isDouble())
            {
                GLOBAL_CRITICAL(tr("Chiave 'npcs' non valida"));
                return Q_NULLPTR;
            }

            npcs.push_back(QPair<QMUD::StaticIdType, bool>(static_cast<QMUD::StaticIdType>(qRound64(elem.toDouble())), true));
        }
    }

    auto itNpcsNoRes = obj.find("npcs_no_res");
    if (itNpcsNoRes != obj.end())
    {
        if (!itNpcsNoRes->isArray())
        {
            GLOBAL_CRITICAL(tr("Chiave 'npcs_no_res' non valida"));
            return Q_NULLPTR;
        }

        QJsonArray jsonArrayNpcs = itNpcsNoRes->toArray();

        for (auto elem : jsonArrayNpcs)
        {
            if (!elem.isDouble())
            {
                GLOBAL_CRITICAL(tr("Chiave 'npcs_no_res' non valida"));
                return Q_NULLPTR;
            }

            npcs.push_back(QPair<QMUD::StaticIdType, bool>(static_cast<QMUD::StaticIdType>(qRound64(elem.toDouble())), false));
        }
    }

    // Items
    auto itItems = obj.find("items");
    if (itItems != obj.end())
    {
        if (!itItems->isArray())
        {
            GLOBAL_CRITICAL(tr("Chiave 'items' non valida"));
            return Q_NULLPTR;
        }

        QJsonArray jsonArrayItems = itItems->toArray();

        for (auto elem : jsonArrayItems)
        {
            if (!elem.isDouble())
            {
                GLOBAL_CRITICAL(tr("Chiave 'items' non valida"));
                return Q_NULLPTR;
            }

            items.push_back(static_cast<QMUD::StaticIdType>(qRound64(elem.toDouble())));
        }
    }

    // Creo la stanza
    Room* room = new Room();
    room->setParent(map);
    room->m_iStaticId = static_cast<QMUD::StaticIdType>(tmpId);
    room->m_strName = tmpName;
    room->m_strDescription = tmpDescription.join(QMUD_GLOBAL_NEW_LINE);
    room->m_exitNorth = exitN;
    room->m_exitSouth = exitS;
    room->m_exitWest = exitW;
    room->m_exitEast = exitE;
    room->m_exitUp = exitU;
    room->m_exitDown = exitD;
    room->m_strInstanceEntryPoint = instanceEntryPoint;
    room->m_iInstanceExitMapStaticId = instanceExitPointMap;
    room->m_iInstanceExitRoomStaticId = instanceExitPointRoom;
    room->m_vFlags = flags;

    room->updateExitsMap();

    return room;
}

Map *Room::map() const
{
    Q_ASSERT(parent());

    return static_cast<Map*>(parent());
}

void Room::removeRIPCharacter(Character *ch)
{
    m_vCharacters.removeAll(ch);
    map()->removeCharacter(ch);
}

void Room::sendLine(Character *ch, QString what, bool canSeeOnly)
{
    auto chs = ch->room()->characters();

    for (Character* chInRoom : chs)
    {
        if (chInRoom != ch && (!canSeeOnly || chInRoom->canSee(ch)))
        {
            if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
            {
                Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
                playerOther->sendLine(what);
            }
        }
    }
}

QMUD::StaticIdType Room::staticId() const
{
    return m_iStaticId;
}

QString Room::name(Entity* suitor) const
{
    Q_UNUSED(suitor)

    return m_strName;
}

QString Room::description(Character *suitor) const
{
        return m_strDescription;

//        // Indicatore della direzione
//        Room *exitToTake = Q_NULLPTR;
//        bool exitToTakeIsOutside = false;
//        QString exitIndicator = "?";
//        float intersectionTopX = 0.0f;
//        bool intersectionTop = false;
//        float intersectionBottomX = 0.0f;
//        bool intersectionBottom = false;
//        float intersectionLeftY = 0.0f;
//        bool intersectionLeft = false;
//        float intersectionRightY = 0.0f;
//        bool intersectionRight = false;

//        if (suitor && suitor->chType() == Character::ChType::CHTYPE_PC)
//        {
//            PcCharacter* pcChar = static_cast<PcCharacter*>(suitor);

//            showResources = pcChar->resourceRadarIsOn();

//            Room * targetRoom = pcChar->tracedRoom();
//            exitToTake = Q_NULLPTR;
//            RoomExit::RoomExitType exitType;

//            if (targetRoom)
//            {
//                // Controllo se sia necessario rieseguire la ricerca del path tra le mappe
//                if (pcChar->tracedRoomLastFromRoom() &&
//                        pcChar->tracedRoomLastFromRoom()->map() == map() &&
//                        pcChar->tracedRoomLastFromRoom()->group() == group())
//                {
//                    Q_ASSERT(pcChar->tracedRoomLastRoomExitToTake());

//                    exitToTake = pcChar->tracedRoomLastRoomExitToTake();
//                    exitType = pcChar->tracedRoomLastRoomExitType();
//                }
//                else
//                {
//                    if (QMUD::qmudWorld->mapsGraph().getPath(this, targetRoom, &exitToTake, exitType))
//                    {
//                        if (!exitToTake)
//                            exitToTake = targetRoom;

//                        pcChar->setTracedRoomLastData(this, exitToTake, exitType);
//                    }
//                }

//                exitToTakeIsOutside = exitToTake && (exitToTake->wildX() > m_iWildX + QMUD_WILD_DESCRIPTION_WIDTH_RADIUS ||
//                                                           exitToTake->wildX() < m_iWildX - QMUD_WILD_DESCRIPTION_WIDTH_RADIUS ||
//                                                           exitToTake->wildY() > m_iWildY + QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS ||
//                                                           exitToTake->wildY() < m_iWildY - QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS);

//                if (!exitToTakeIsOutside && exitToTake && targetRoom != exitToTake && exitToTake->map() == map() && exitToTake->group() == group())
//                {
//                    if (exitType == RoomExit::RoomExitType::EXIT_NORTH)
//                        exitIndicator = "^";
//                    else if (exitType == RoomExit::RoomExitType::EXIT_SOUTH)
//                        exitIndicator = "v";
//                    else if (exitType == RoomExit::RoomExitType::EXIT_WEST)
//                        exitIndicator = ">";
//                    else if (exitType == RoomExit::RoomExitType::EXIT_EAST)
//                        exitIndicator = "<";
//                    else
//                        exitIndicator = "o";
//                }

//                // Calcolo intersezioni con i bordi della mappa
//                if (exitToTakeIsOutside)
//                {
//                    if (m_iWildX == exitToTake->wildX() && exitToTake->wildY() < m_iWildY)
//                    {
//                        intersectionTopX = static_cast<float>(m_iWildX);
//                        intersectionTop = true;
//                    }
//                    else if (m_iWildY != exitToTake->wildY() && exitToTake->wildY() < m_iWildY)
//                    {
//                        intersectionTopX = ((-QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS)
//                                            / static_cast<float>(exitToTake->wildY() - m_iWildY)
//                                            * (exitToTake->wildX() - m_iWildX))
//                                + m_iWildX + 0.5f;
//                        intersectionTop = true;
//                    }

//                    if (m_iWildX == exitToTake->wildX() && exitToTake->wildY() > m_iWildY)
//                    {
//                        intersectionBottomX = static_cast<float>(m_iWildX);
//                        intersectionBottom = true;
//                    }
//                    else if (m_iWildY != exitToTake->wildY() && exitToTake->wildY() > m_iWildY)
//                    {
//                        intersectionBottomX = ((QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS + 1.0f)
//                                               / static_cast<float>(exitToTake->wildY() - m_iWildY)
//                                               * (exitToTake->wildX() - m_iWildX))
//                                + m_iWildX + 0.5f;
//                        intersectionBottom = true;
//                    }

//                    if (m_iWildY == exitToTake->wildY() && exitToTake->wildX() < m_iWildX)
//                    {
//                        intersectionLeftY = static_cast<float>(m_iWildY);
//                        intersectionLeft = true;
//                    }
//                    else if (m_iWildX != exitToTake->wildX() && exitToTake->wildX() < m_iWildX)
//                    {
//                        intersectionLeftY = ((-QMUD_WILD_DESCRIPTION_WIDTH_RADIUS)
//                                             / static_cast<float>(exitToTake->wildX() - m_iWildX)
//                                             * (exitToTake->wildY() - m_iWildY))
//                                + m_iWildY + 0.5f;
//                        intersectionLeft = true;
//                    }

//                    if (m_iWildY == exitToTake->wildY() && exitToTake->wildX() > m_iWildX)
//                    {
//                        intersectionRightY = static_cast<float>(m_iWildY);
//                        intersectionRight = true;
//                    }
//                    else if (m_iWildX != exitToTake->wildX() && exitToTake->wildX() > m_iWildX)
//                    {
//                        intersectionRightY = ((QMUD_WILD_DESCRIPTION_WIDTH_RADIUS + 1.0f)
//                                              / static_cast<float>(exitToTake->wildX() - m_iWildX)
//                                              * (exitToTake->wildY() - m_iWildY))
//                                + m_iWildY + 0.5f;
//                        intersectionRight = true;
//                    }
//                }
//            }
//        }

//        QString description;

//        char lastChar = '\0';
//        QDateTime now = QDateTime::currentDateTime();
//        bool isDark = flagCheck(QMUD::RoomFlagType::OUTSIDE) && DateTime::isDark();
//        bool isAlmostDark = flagCheck(QMUD::RoomFlagType::OUTSIDE) && DateTime::isAlmostDark();

//        for (int y = -QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS; y <= QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS; y++)
//        {
//            for (int x = -QMUD_WILD_DESCRIPTION_WIDTH_RADIUS; x <= QMUD_WILD_DESCRIPTION_WIDTH_RADIUS; x++)
//            {
//                char currChar = m_strDescription.at((y + QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS)  * QMUD_WILD_DESCRIPTION_WIDTH + x + QMUD_WILD_DESCRIPTION_WIDTH_RADIUS).toLatin1();
//                bool darkLocation = false;

//                if ((isDark && QMUD::WildDarkKernel[y+QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS][x+QMUD_WILD_DESCRIPTION_WIDTH_RADIUS] == 0) ||
//                    (isAlmostDark && QMUD::WildAlmostDarkKernel[y+QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS][x+QMUD_WILD_DESCRIPTION_WIDTH_RADIUS] == 0))
//                    darkLocation = true;

//                bool drawTargetRoomIndicator = false;
//                QString targetRoomIndicator;

//                if (exitToTakeIsOutside && (x == QMUD_WILD_DESCRIPTION_WIDTH_RADIUS || x == -QMUD_WILD_DESCRIPTION_WIDTH_RADIUS ||
//                        y == QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS || y == -QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS))
//                {
//                    int tmpRow = m_iWildY + y;
//                    int tmpColumn = m_iWildX + x;

//                    if (y == -QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS && intersectionTop &&
//                            intersectionTopX >= tmpColumn && intersectionTopX <= tmpColumn + 1)
//                    {
//                        targetRoomIndicator = "^";
//                        drawTargetRoomIndicator = true;
//                    }
//                    else if (y == QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS && intersectionBottom &&
//                            intersectionBottomX >= tmpColumn && intersectionBottomX <= tmpColumn + 1)
//                    {
//                        targetRoomIndicator = "v";
//                        drawTargetRoomIndicator = true;
//                    }
//                    else if (x == -QMUD_WILD_DESCRIPTION_WIDTH_RADIUS && intersectionLeft &&
//                            intersectionLeftY >= tmpRow && intersectionLeftY <= tmpRow + 1)
//                    {
//                        targetRoomIndicator = "<";
//                        drawTargetRoomIndicator = true;
//                    }
//                    else if (x == QMUD_WILD_DESCRIPTION_WIDTH_RADIUS && intersectionRight &&
//                            intersectionRightY >= tmpRow && intersectionRightY <= tmpRow + 1)
//                    {
//                        targetRoomIndicator = ">";
//                        drawTargetRoomIndicator = true;
//                    }
//                }

//                if (currChar == ' ')
//                {
//                    if (lastChar == '\0')
//                        description += "&d";

//                    int tmpRow = m_iWildY + y;
//                    int tmpColumn = m_iWildX + x;

//                    if (exitToTake && exitToTake->wildX() == tmpColumn && exitToTake->wildY() == tmpRow)
//                    {
//                        description += "{r&Y" + exitIndicator;
//                        lastChar = '\0';
//                    }
//                    else if (drawTargetRoomIndicator)
//                    {
//                        description += "{r&Y" + targetRoomIndicator;
//                        lastChar = '\0';
//                    }
//                    else
//                        description += " ";
//                }
//                else if (x == 0 && y == 0)
//                {
//                    if (lastChar == '\0')
//                        description += "&d";

//                    description += QMUD::wildFromCharToBackgroundColor(currChar, map()) + "&WX";
//                    lastChar = '\0';
//                }
//                else
//                {
//                    int tmpRow = m_iWildY + y;
//                    int tmpColumn = m_iWildX + x;

//                    if (exitToTake && exitToTake->wildX() == tmpColumn && exitToTake->wildY() == tmpRow)
//                    {
//                        description += "{r&Y" + exitIndicator;
//                        lastChar = '\0';
//                    }
//                    else if (drawTargetRoomIndicator)
//                    {
//                        description += "{r&Y" + targetRoomIndicator;
//                        lastChar = '\0';
//                    }
//                    else
//                    {
//                        Room *room = map()->room(tmpColumn, tmpRow);

//                        if (room)
//                        {
//                            bool containsPC = false;
//                            bool containsNPC = false;
//                            bool containsSpecificNpc = false;
//                            QMUD::StaticIdType specificNpcMapId = QMUD::StaticIdInvalid;
//                            QMUD::StaticIdType specificNpcId = QMUD::StaticIdInvalid;

//                            if (suitor->chType() == Character::ChType::CHTYPE_PC)
//                            {
//                                PcCharacter* pcChar = static_cast<PcCharacter*>(suitor);

//                                specificNpcMapId = pcChar->tracedRoomNpcMapId();
//                                specificNpcId = pcChar->tracedRoomNpcId();
//                            }

//                            if (lastChar == '\0')
//                                description += "&d";

//                            if (darkLocation)
//                            {
//                                description += "&z" + QMUD::wildFromCharToDescription(currChar, false, map());
//                                lastChar = '\0';
//                            }
//                            else if (room->containsHostileNPC(suitor, containsNPC, containsPC, specificNpcMapId, specificNpcId, containsSpecificNpc))
//                            {
//                                if (containsSpecificNpc)
//                                    description += "{r";

//                                description += QMUD::wildFromCharToBackgroundColor(currChar, map()) + "&Rx";
//                                lastChar = '\0';
//                            }
//                            else if (containsNPC)
//                            {
//                                if (containsSpecificNpc)
//                                    description += "{r";

//                                description += QMUD::wildFromCharToBackgroundColor(currChar, map()) + "&Yx";
//                                lastChar = '\0';
//                            }
//                            else if (containsPC)
//                            {
//                                if (containsSpecificNpc)
//                                    description += "{r";

//                                description += QMUD::wildFromCharToBackgroundColor(currChar, map()) + "&Px";
//                                lastChar = '\0';
//                            }
//                            else if (showResources && room->containsResources(now))
//                            {
//                                description += QMUD::wildFromCharToBackgroundColor(currChar, map()) + "&Cr";
//                                lastChar = '\0';
//                            }
//                            else
//                            {
//                                description += QMUD::wildFromCharToDescription(currChar, currChar != lastChar, map());
//                                currChar = lastChar;
//                            }
//                        }
//                        else
//                        {
//                            if (lastChar == '\0')
//                                description += "&d";

//                            description += "&a{[235]";

//                            if (darkLocation)
//                            {
//                                description += "&z" + QMUD::wildFromCharToDescription(currChar, false, map());
//                                lastChar = '\0';
//                            }
//                            else
//                            {
//                                description += QMUD::wildFromCharToDescription(currChar, currChar != lastChar, map());
//                                currChar = lastChar;
//                            }
//                        }
//                    }
//                }
//            }

//            description += QMUD_GLOBAL_NEW_LINE;
//            description += "&d";
//        }

        //        return description;
}

QVector<QMUD::ClientDataRoomInfo::ChInfo> Room::clientDataNpcs(PcCharacter* pc) const
{
    QVector<QMUD::ClientDataRoomInfo::ChInfo> chs;

    for (int y = -QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS; y <= QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS; y++)
    {
        for (int x = -QMUD_WILD_DESCRIPTION_WIDTH_RADIUS; x <= QMUD_WILD_DESCRIPTION_WIDTH_RADIUS; x++)
        {
            int tmpRow = m_iWildY + y;
            int tmpColumn = m_iWildX + x;

            Room *room = map()->room(tmpColumn, tmpRow, m_iWildMapIndex);

            if (room)
            {
                for (auto it = room->children().begin(); it != room->children().end(); ++it)
                {
                    Character* ch = qobject_cast<Character*>(*it);

                    if (ch && ch != pc && pc->canSee(ch))
                    {
                        QMUD::ClientDataRoomInfo::ChInfo chInfo;
                        chInfo.x = tmpColumn;
                        chInfo.y = tmpRow;
                        chInfo.id = ch->id();
                        chInfo.name = ch->name(Q_NULLPTR);
                        chInfo.race = ch->race();
                        chInfo.type = ch->chType() == Character::ChType::CHTYPE_NPC
                                ? (static_cast<NpcCharacter*>(ch)->flagCheck(QMUD::NpcCharacterFlagType::AGGRESSIVE)
                                   ? QMUD::ClientDataRoomInfo::ChType::NPC_AGGRESSIVE
                                   : QMUD::ClientDataRoomInfo::ChType::NPC)
                                : QMUD::ClientDataRoomInfo::ChType::PLAYER;
                        chs.push_back(chInfo);
                    }
                }
            }
        }
    }

    return chs;
}


bool Room::isWilderness() const
{
    return m_bIsWildDescription;
}

bool Room::canContainResources() const
{
    return !flagCheck(QMUD::RoomFlagType::NO_RESOURCES);
}

int Room::wildX() const
{
    return m_iWildX;
}

int Room::wildY() const
{
    return m_iWildY;
}

QMUD::StaticIdType Room::wildMapIndex() const
{
    return m_iWildMapIndex;
}

int Room::group() const
{
    return m_iGroup;
}

int Room::wildGroup() const
{
    return m_iWildGroup;
}

void Room::setGroup(int group)
{
    m_iGroup = group;
}

const RoomExit &Room::exitNorth() const
{
    return m_exitNorth;
}

const RoomExit &Room::exitSouth() const
{
    return m_exitSouth;
}

const RoomExit &Room::exitWest() const
{
    return m_exitWest;
}

const RoomExit &Room::exitEast() const
{
    return m_exitEast;
}

const RoomExit &Room::exitUp() const
{
    return m_exitUp;
}

const RoomExit &Room::exitDown() const
{
    return m_exitDown;
}

QVector<RoomExit *> Room::exits()
{
    return m_vExits;
}

RoomExit &Room::exitNorth()
{
    return m_exitNorth;
}

RoomExit &Room::exitSouth()
{
    return m_exitSouth;
}

RoomExit &Room::exitWest()
{
    return m_exitWest;
}

RoomExit &Room::exitEast()
{
    return m_exitEast;
}

RoomExit &Room::exitUp()
{
    return m_exitUp;
}

RoomExit &Room::exitDown()
{
    return m_exitDown;
}

void Room::removeExit(RoomExit::RoomExitType exitType)
{
    for (auto e : m_vExits)
    {
        if (e->exitType() == exitType)
        {
            e->setIsValid(false);
            updateExitsMap();
            return;
        }
    }
}

QString Room::instanceEntryPoint() const
{
    return m_strInstanceEntryPoint;
}

Room *Room::instanceExitRoom() const
{
    return m_ptrInstanceExitRoom;
}

void Room::instanceExitRoomStaticId(QMUD::StaticIdType &mapStaticId, QMUD::StaticIdType &roomStaticId) const
{
    mapStaticId = m_iInstanceExitMapStaticId;
    roomStaticId = m_iInstanceExitRoomStaticId;
}

void Room::setInstanceExitRoom(Room *room)
{
    m_ptrInstanceExitRoom = room;
}

QList<Character *> Room::characters() const
{
    return m_vCharacters;
}

Character *Room::character(QMUD::IdType id) const
{
    for (auto ch : m_vCharacters)
        if (ch->id() == id)
            return ch;

    return Q_NULLPTR;
}

bool Room::containsCharacters(Entity* suitor) const
{
    if (suitor && suitor->entityType() == EntityType::CHARACTER)
    {
        Character* chSuitor = static_cast<Character*>(suitor);

        for (auto it = children().begin(); it != children().end(); ++it)
        {
            Character* ch = qobject_cast<Character*>(*it);

            if (ch && chSuitor->canSee(ch))
                return true;
        }
    }
    else
    {
        for (auto it = children().begin(); it != children().end(); ++it)
        {
            Character* ch = qobject_cast<Character*>(*it);

            if (ch)
                return true;
        }
    }

    return false;
}

bool Room::containsHostileNPC(Entity *suitor, bool& containsNPC, bool& containsPC,
                              QMUD::StaticIdType mapId, QMUD::StaticIdType npcId, bool& containsSpecificNpc) const
{
    containsNPC = false;
    containsPC = false;
    containsSpecificNpc = false;

    bool ret = false;

    if (suitor && suitor->entityType() == EntityType::CHARACTER)
    {
        Character* chSuitor = static_cast<Character*>(suitor);

        for (auto it = children().begin(); it != children().end(); ++it)
        {
            Character* ch = qobject_cast<Character*>(*it);

            if (ch &&
                    ch->chType() == Character::ChType::CHTYPE_NPC &&
                    static_cast<NpcCharacter*>(ch)->flagCheck(QMUD::NpcCharacterFlagType::AGGRESSIVE) &&
                    chSuitor->canSee(ch))
                ret = true;
            else if (ch && ch->chType() == Character::ChType::CHTYPE_NPC)
            {
                containsNPC = true;

                NpcCharacter* npc = static_cast<NpcCharacter*>(ch);

                if (mapId == npc->mapStaticId() && npcId == npc->staticId())
                    containsSpecificNpc = true;
            }
            else if (ch && ch->chType() == Character::ChType::CHTYPE_PC)
                containsPC = true;
        }
    }

    return ret;
}

bool Room::containsResources(QDateTime now) const
{
    for (auto it = children().begin(); it != children().end(); ++it)
    {
        ItemResource* item = qobject_cast<ItemResource*>(*it);

        if (item && item->collectable(now))
            return true;
    }

    return false;
}

QList<Entity *> Room::entities(Entity *suitor) const
{
    QList<Entity*> list;

    if (suitor && suitor->entityType() == EntityType::CHARACTER)
    {
        Character* ch = static_cast<Character*>(suitor);

        for (auto it = children().begin(); it != children().end(); ++it)
        {
            Entity* e = qobject_cast<Entity*>(*it);

            if (e->entityType() != EntityType::CHARACTER ||
                ch->canSee(static_cast<Character*>(e)))
            {
                list.push_back(e);
            }
        }
    }
    else
    {
        for (auto it = children().begin(); it != children().end(); ++it)
        {
            Entity* e = qobject_cast<Entity*>(*it);

            if (e)
                list.push_back(e);
        }
    }

    return list;
}

QMUD::IdType Room::currMark() const
{
    return m_iMark;
}

void Room::setMark(QMUD::IdType mark)
{
    m_iMark = mark;
}

bool Room::flagCheck(QMUD::RoomFlagType flag) const
{
    return m_vFlags.contains(flag);
}

QVector<QMUD::RoomFlagType> Room::flags() const
{
    return m_vFlags;
}

void Room::updateExitsMap()
{
    m_vExits.clear();

    if (m_exitNorth.isValid())  m_vExits.push_back(&m_exitNorth);
    if (m_exitSouth.isValid())  m_vExits.push_back(&m_exitSouth);
    if (m_exitEast.isValid())   m_vExits.push_back(&m_exitEast);
    if (m_exitWest.isValid())   m_vExits.push_back(&m_exitWest);
    if (m_exitUp.isValid())     m_vExits.push_back(&m_exitUp);
    if (m_exitDown.isValid())   m_vExits.push_back(&m_exitDown);

    m_vExits.squeeze();
}

void Room::childEvent(QChildEvent *event)
{
    if (qobject_cast<Character*>(event->child()))
    {
        Character* ch = qobject_cast<Character*>(event->child());

        if (event->added())
        {
            m_vCharacters.push_back(ch);
            map()->addCharacter(qobject_cast<Character*>(event->child()));
        }
        else if (event->removed())
        {
            m_vCharacters.removeAll(ch);
            map()->removeCharacter(qobject_cast<Character*>(event->child()));
        }
    }
}
