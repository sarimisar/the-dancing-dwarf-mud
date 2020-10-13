/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef ROOM_H
#define ROOM_H

#include "entity.h"
#include "roomexit.h"

#include <QVector>
#include <QMap>
#include <QLinkedList>


namespace CORE
{

class Character;
class PcCharacter;
class Map;

class Room : public Entity
{
    Q_OBJECT

    friend class Map;

public:
    virtual ~Room() Q_DECL_OVERRIDE;

    static Room* loadRoom(const QJsonObject &obj, Map* map, QVector<QPair<QMUD::StaticIdType, bool> > &npcs, QVector<QMUD::StaticIdType> &items);

    Map* map() const;

    void removeRIPCharacter(Character* ch);
    void sendLine(Character* ch, QString what, bool canSeeOnly = true);

    QMUD::StaticIdType staticId() const;
    QString name(Entity* suitor = Q_NULLPTR) const Q_DECL_OVERRIDE;
    QString description(Character* suitor) const;
    QVector<QMUD::ClientDataRoomInfo::ChInfo> clientDataNpcs(CORE::PcCharacter *pc) const;
    bool isWilderness() const;
    bool canContainResources() const;
    int wildX() const;
    int wildY() const;
    QMUD::StaticIdType wildMapIndex() const;

    int group() const;
    int wildGroup() const;
    void setGroup(int group);

    const RoomExit& exitNorth() const;
    const RoomExit& exitSouth() const;
    const RoomExit& exitWest() const;
    const RoomExit& exitEast() const;
    const RoomExit& exitUp() const;
    const RoomExit& exitDown() const;
    QVector<RoomExit *> exits();

    RoomExit& exitNorth();
    RoomExit& exitSouth();
    RoomExit& exitWest();
    RoomExit& exitEast();
    RoomExit& exitUp();
    RoomExit& exitDown();

    void removeExit(RoomExit::RoomExitType exitType);

    QString instanceEntryPoint() const;
    Room* instanceExitRoom() const;
    void instanceExitRoomStaticId(QMUD::StaticIdType& mapStaticId, QMUD::StaticIdType& roomStaticId) const;
    void setInstanceExitRoom(Room* room);

    QList<Character*> characters() const;
    Character* character(QMUD::IdType id) const;
    bool containsCharacters(Entity* suitor) const;
    bool containsHostileNPC(Entity *suitor, bool &containsNPC, bool &containsPC,
                            QMUD::StaticIdType mapId, QMUD::StaticIdType npcId, bool &containsSpecificNpc) const;
    bool containsResources(QDateTime now) const;
    QList<Entity*> entities(Entity *suitor) const;

    QMUD::IdType currMark() const;
    void setMark(QMUD::IdType mark);

    bool flagCheck(QMUD::RoomFlagType flag) const;
    QVector<QMUD::RoomFlagType> flags() const;

signals:

public slots:

protected:
    virtual void childEvent(QChildEvent *event) Q_DECL_OVERRIDE;

private:
    explicit Room(Map *parent = Q_NULLPTR);

    void updateExitsMap();

    QMUD::StaticIdType m_iStaticId;
    QString m_strName;
    QString m_strDescription;

    bool m_bIsWildDescription;
    int m_iWildX;
    int m_iWildY;
    int m_iWildGroup;
    QMUD::StaticIdType m_iWildMapIndex;

    int m_iGroup;

    RoomExit m_exitNorth;
    RoomExit m_exitSouth;
    RoomExit m_exitWest;
    RoomExit m_exitEast;
    RoomExit m_exitUp;
    RoomExit m_exitDown;

    QVector<RoomExit *> m_vExits;

    QString m_strInstanceEntryPoint;

    QMUD::IdType m_iMark;

    QMUD::StaticIdType m_iInstanceExitMapStaticId;
    QMUD::StaticIdType m_iInstanceExitRoomStaticId;
    Room *m_ptrInstanceExitRoom;

    QVector<QMUD::RoomFlagType> m_vFlags;
    QList<Character*> m_vCharacters;
};

} // namespace CORE

#endif // ROOM_H
