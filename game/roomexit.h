/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef ROOMEXIT_H
#define ROOMEXIT_H

#include "entity.h"

#include <QJsonObject>

namespace CORE
{

class Map;
class Room;

class RoomExit
{
    friend class Room;
    friend class Map;
    friend class World;

public:
    enum class RoomExitType
    {
        EXIT_NORTH,
        EXIT_SOUTH,
        EXIT_WEST,
        EXIT_EAST,
        EXIT_UP,
        EXIT_DOWN
    };

    RoomExit();

    static QString roomExitToReadableString(RoomExitType exit);
    QString roomExitToReadableString();

    RoomExitType exitType() const;

    static RoomExit loadExit(const QJsonObject &obj, RoomExitType exitType);

    bool isValid() const;

    void initExit(Room* room, RoomExit* opposedExit);

    Room* exitRoom() const;

    QString code() const;

    QMUD::ActionType openAction() const;
    QMUD::ActionType closeAction() const;
    bool isOpen() const;
    void toggle();
    bool isHidden() const;
    QString door() const;
    bool isDoor() const;
    QString descOpen() const;
    QString descClose() const;
    QString descOpenOther() const;
    QString descCloseOther() const;

    bool isPatrolBarrier() const;

private:
    void setIsValid(bool isValid);

    QString m_strCode;
    QMUD::StaticIdType m_iMapStaticId;
    QMUD::StaticIdType m_iRoomStaticId;

    bool m_bIsValid;
    QMUD::ActionType m_eOpenAction;
    QMUD::ActionType m_eCloseAction;
    bool m_bIsOpen;
    bool m_bIsHidden;
    QString m_strDoor;
    bool m_bIsDoor;
    QString m_strDescOpen;
    QString m_strDescClose;
    QString m_strDescOpenOther;
    QString m_strDescCloseOther;
    bool m_bIsPatrolBarrier;

    Room* m_ptrRoom;
    RoomExit* m_ptrOpposedRoomExit;
    RoomExitType m_eExitType;
};

} // namespace CORE

#endif // ROOMEXIT_H
