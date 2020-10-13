/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "roomexit.h"
#include "room.h"
#include "../global.h"
#include "../globaldebug.h"

using namespace CORE;

RoomExit::RoomExit() :
    m_iMapStaticId(QMUD::StaticIdInvalid),
    m_iRoomStaticId(QMUD::StaticIdInvalid),
    m_bIsValid(false),
    m_eOpenAction(QMUD::ActionType::OPEN),
    m_eCloseAction(QMUD::ActionType::CLOSE),
    m_bIsOpen(false),
    m_bIsHidden(false),
    m_strDoor(QObject::tr("porta")),
    m_bIsDoor(false),
    m_bIsPatrolBarrier(false),
    m_ptrRoom(Q_NULLPTR),
    m_ptrOpposedRoomExit(Q_NULLPTR),
    m_eExitType(RoomExitType::EXIT_NORTH)
{

}

QString RoomExit::roomExitToReadableString(RoomExitType exit)
{
    switch (exit)
    {
        case RoomExitType::EXIT_NORTH:  return QObject::tr("nord");
        case RoomExitType::EXIT_SOUTH:  return QObject::tr("sud");
        case RoomExitType::EXIT_WEST:   return QObject::tr("ovest");
        case RoomExitType::EXIT_EAST:   return QObject::tr("est");
        case RoomExitType::EXIT_UP:     return QObject::tr("alto");
        case RoomExitType::EXIT_DOWN:   return QObject::tr("basso");
    }

    Q_ASSERT(false);
    return "";
}

QString RoomExit::roomExitToReadableString()
{
    return roomExitToReadableString(m_eExitType);
}

RoomExit::RoomExitType RoomExit::exitType() const
{
    return m_eExitType;
}

RoomExit RoomExit::loadExit(const QJsonObject &obj, RoomExitType exitType)
{
    // RoomCode
    QString strTmpRoomCode;
    if (!QMUD::fileJsonRead(obj, "code", strTmpRoomCode, true))
        return RoomExit();

    // Action
    QString tmpStr;
    QMUD::ActionType tmpOpenAction = QMUD::ActionType::OPEN;
    QMUD::ActionType tmpCloseAction = QMUD::ActionType::CLOSE;
    if (!QMUD::fileJsonRead(obj, "action", tmpStr, false, true))
        return RoomExit();

    if (!tmpStr.isEmpty())
    {
        tmpOpenAction = QMUD::actionFromString(tmpStr);
        tmpCloseAction = tmpOpenAction;
    }
    else
    {
        if (!QMUD::fileJsonRead(obj, "openAction", tmpStr, false, true))
            return RoomExit();

        if (!tmpStr.isEmpty())
            tmpOpenAction = QMUD::actionFromString(tmpStr);

        if (!QMUD::fileJsonRead(obj, "closeAction", tmpStr, false, true))
            return RoomExit();

        if (!tmpStr.isEmpty())
            tmpCloseAction = QMUD::actionFromString(tmpStr);
    }

    if (tmpOpenAction == QMUD::ActionType::UNKNOWN ||
        tmpCloseAction == QMUD::ActionType::UNKNOWN)
        return RoomExit();

    // IsOpen
    bool tmpIsOpen;
    if (!QMUD::fileJsonRead(obj, "isOpen", tmpIsOpen, true, false))
        return RoomExit();

    // IsHidden
    bool tmpIsHidden;
    if (!QMUD::fileJsonRead(obj, "isHidden", tmpIsHidden, true, false))
        return RoomExit();

    // DescOpen
    QString tmpDescOpen;
    if (!QMUD::fileJsonRead(obj, "descOpen", tmpDescOpen, false, true, ""))
        return RoomExit();

    // DescClose
    QString tmpDescClose;
    if (!QMUD::fileJsonRead(obj, "descClose", tmpDescClose, false, true, ""))
        return RoomExit();

    // DescOpenOther
    QString tmpDescOpenOther;
    if (!QMUD::fileJsonRead(obj, "descOpenOther", tmpDescOpenOther, false, true, ""))
        return RoomExit();

    // DescCloseOther
    QString tmpDescCloseOther;
    if (!QMUD::fileJsonRead(obj, "descCloseOther", tmpDescCloseOther, false, true, ""))
        return RoomExit();

    // Door
    QString tmpDoor;
    if (!QMUD::fileJsonRead(obj, "door", tmpDoor, false, true, ""))
        return RoomExit();

    // IsPatrolBarrier
    bool tmpIsPatrolBarrier;
    if (!QMUD::fileJsonRead(obj, "isPatrolBarrier", tmpIsPatrolBarrier, true, false))
        return RoomExit();

    RoomExit exit;
    exit.m_bIsValid = true;
    exit.m_strCode = strTmpRoomCode;
    exit.m_eOpenAction = tmpOpenAction;
    exit.m_eCloseAction = tmpCloseAction;
    exit.m_bIsOpen = tmpIsOpen;
    exit.m_bIsHidden = tmpIsHidden;
    exit.m_strDoor = tmpDoor;
    exit.m_strDescOpen = tmpDescOpen;
    exit.m_strDescClose = tmpDescClose;
    exit.m_strDescOpenOther = tmpDescOpenOther;
    exit.m_strDescCloseOther = tmpDescCloseOther;
    exit.m_bIsDoor = !tmpDoor.isEmpty();
    exit.m_eExitType = exitType;
    exit.m_bIsPatrolBarrier = tmpIsPatrolBarrier;

    return exit;
}

bool RoomExit::isValid() const
{
    return m_bIsValid;
}

void RoomExit::setIsValid(bool isValid)
{
    m_bIsValid = isValid;
}

Room *RoomExit::exitRoom() const
{
    return m_ptrRoom;
}

QString RoomExit::code() const
{
    return m_strCode;
}

void RoomExit::initExit(Room *room, RoomExit *opposedExit)
{
    m_ptrRoom = room;
    m_ptrOpposedRoomExit = opposedExit;
}

QMUD::ActionType RoomExit::openAction() const
{
    return m_eOpenAction;
}

QMUD::ActionType RoomExit::closeAction() const
{
    return m_eCloseAction;
}

bool RoomExit::isOpen() const
{
    return m_bIsOpen || !isDoor();
}

void RoomExit::toggle()
{
    m_bIsOpen = !m_bIsOpen;

    if (m_ptrOpposedRoomExit && m_ptrOpposedRoomExit->isValid())
        m_ptrOpposedRoomExit->m_bIsOpen = m_bIsOpen;
}

bool RoomExit::isHidden() const
{
    return m_bIsHidden;
}

QString RoomExit::door() const
{
    return m_strDoor;
}

bool RoomExit::isDoor() const
{
    return m_bIsDoor;
}

QString RoomExit::descOpen() const
{
    return m_strDescOpen;
}

QString RoomExit::descClose() const
{
    return m_strDescClose;
}

QString RoomExit::descOpenOther() const
{
    return m_strDescOpenOther;
}

QString RoomExit::descCloseOther() const
{
    return m_strDescCloseOther;
}

bool RoomExit::isPatrolBarrier() const
{
    return m_bIsPatrolBarrier;
}
