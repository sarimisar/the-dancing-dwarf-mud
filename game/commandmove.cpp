/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandmove.h"
#include "world.h"
#include "map.h"
#include "room.h"
#include "../globaldebug.h"
#include "npccharacter.h"

using namespace CORE;

CommandMove::CommandMove() :
    Command()
{

}

CommandMove::~CommandMove()
{

}

bool CommandMove::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("nord"), cmd) ||
            Command::isOf(QObject::tr("sud"), cmd) ||
            Command::isOf(QObject::tr("ovest"), cmd) ||
            Command::isOf(QObject::tr("est"), cmd) ||
            Command::isOf(QObject::tr("alto"), cmd) ||
            Command::isOf(QObject::tr("basso"), cmd);
}

QStringList CommandMove::commands()
{
    return QStringList() << QObject::tr("nord")
                         << QObject::tr("sud")
                         << QObject::tr("ovest")
                         << QObject::tr("est")
                         << QObject::tr("alto")
                         << QObject::tr("basso");
}

void CommandMove::sendUsage(Player *player, bool help)
{
    Q_UNUSED(player)
    Q_UNUSED(help)
}

void CommandMove::executePc(Character *ch, Player* player)
{
    Q_UNUSED(ch)

    QSet<Player*> forceLook;

    auto targetCh = ch->isInCombat() ? ch->targetCharacter(true) : Q_NULLPTR;

    if (targetCh)
    {
        if (ch->chType() == Character::ChType::CHTYPE_NPC)
        {
            return;
        }
        else if (player->level() == Player::PlayerLevel::PLAYER_LEVEL_NORMAL)
        {
            player->sendLine(QObject::tr("Stai combattendo!"));
            return;
        }
    }

    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    Room* room = ch->room();

    QString moveCmd = command();
    QString moveStr;
    QString arriveStr;
    RoomExit exit;

    if (QObject::tr("nord").startsWith(moveCmd[0]))
    {
        exit = room->exitNorth();
        moveStr = QObject::tr("nord");
        arriveStr = QObject::tr("sud");
    }
    else if (QObject::tr("sud").startsWith(moveCmd[0]))
    {
        exit = room->exitSouth();
        moveStr = QObject::tr("sud");
        arriveStr = QObject::tr("nord");
    }
    else if (QObject::tr("ovest").startsWith(moveCmd[0]))
    {
        exit = room->exitWest();
        moveStr = QObject::tr("ovest");
        arriveStr = QObject::tr("est");
    }
    else if (QObject::tr("est").startsWith(moveCmd[0]))
    {
        exit = room->exitEast();
        moveStr = QObject::tr("est");
        arriveStr = QObject::tr("ovest");
    }
    else if (QObject::tr("alto").startsWith(moveCmd[0]))
    {
        exit = room->exitUp();
        moveStr = QObject::tr("alto");
        arriveStr = QObject::tr("basso");
    }
    else if (QObject::tr("basso").startsWith(moveCmd[0]))
    {
        exit = room->exitDown();
        moveStr = QObject::tr("basso");
        arriveStr = QObject::tr("alto");
    }

    if (!exit.isValid())
    {
        if (player)
        {
            if (moveStr == QObject::tr("alto") || moveStr == QObject::tr("basso"))
                player->sendLine(QObject::tr("Non puoi andare in %1").arg(moveStr));
            else
                player->sendLine(QObject::tr("Non puoi andare a %1").arg(moveStr));
        }
        return;
    }

    if (!exit.isOpen())
    {
        if (player)
        {
            if (exit.isHidden())
            {
                if (moveStr == QObject::tr("alto") || moveStr == QObject::tr("basso"))
                    player->sendLine(QObject::tr("Non puoi andare in %1").arg(moveStr));
                else
                    player->sendLine(QObject::tr("Non puoi andare a %1").arg(moveStr));
            }
            else
                player->sendLine(QObject::tr("E' chiuso"));
        }
        return;
    }

    Room* exitRoom = exit.exitRoom();

    if ((exitRoom->flagCheck(QMUD::RoomFlagType::IMPERVIOUS) ||
         exitRoom->flagCheck(QMUD::RoomFlagType::DIFFICULT) ||
         exitRoom->flagCheck(QMUD::RoomFlagType::IMPOSSIBLE)) &&
            ch->chType() == Character::ChType::CHTYPE_PC)
    {
        PcCharacter *pch = static_cast<PcCharacter*>(ch);

        if (pch->moveCounter() > 20)
        {
            player->sendLine(QObject::tr("Pant! pant! troppo veloce!"));
            return;
        }

        pch->updateMoveCounter();
    }

    if (exitRoom->flagCheck(QMUD::RoomFlagType::IMPOSSIBLE) &&
            !ch->hasTemporaryStatus(QMUD::TemporaryStatusType::FLY))
    {
        if (player)
            player->sendLine(QObject::tr("Dovresti poter volare!"));
        return;
    }

    int actionPoints = 1;

    if (!ch->hasTemporaryStatus(QMUD::TemporaryStatusType::FLY))
    {
        if (exitRoom->flagCheck(QMUD::RoomFlagType::IMPERVIOUS))
            actionPoints = 6;
        else if (exitRoom->flagCheck(QMUD::RoomFlagType::DIFFICULT))
            actionPoints = 2;
        else
            actionPoints = 1;
    }

    if (ch->actionPointsCurrent() < actionPoints)
    {
        if (player)
            player->sendLine(QObject::tr("Pant! pant! hai bisogno di riposo!"));
        return;
    }

    ch->useActionPoint(actionPoints);
    ch->setParent(Q_NULLPTR);

    auto characters = room->characters();

    QString msgOther;

    if (moveStr == QObject::tr("alto") || moveStr == QObject::tr("basso"))
    {
        if (ch->hasTemporaryStatus(QMUD::TemporaryStatusType::FLY))
            msgOther = QObject::tr("%1 vola in %2").arg(ch->name(Q_NULLPTR)).arg(moveStr);
        else
            msgOther = QObject::tr("%1 va in %2").arg(ch->name(Q_NULLPTR)).arg(moveStr);
    }
    else
    {
        if (ch->hasTemporaryStatus(QMUD::TemporaryStatusType::FLY))
            msgOther = QObject::tr("%1 vola a %2").arg(ch->name(Q_NULLPTR)).arg(moveStr);
        else
            msgOther = QObject::tr("%1 va a %2").arg(ch->name(Q_NULLPTR)).arg(moveStr);
    }

    for (Character* chInRoom : characters)
    {
        if (chInRoom != ch && chInRoom->canSee(ch))
        {
            if (chInRoom->chType() == Character::ChType::CHTYPE_PC &&
                    chInRoom->chFollowed() != ch->id())
            {
                Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
                playerOther->sendLine(msgOther);

                forceLook.insert(playerOther);
            }
        }
    }

    ch->setParent(exitRoom);

	// Food and Drink
    if (ch->currentFoodIsEating())
	{
		if (player)
            player->sendLine(QObject::tr("&OSmetti di mangiare %1&d").arg(ch->currentFood()));

		ch->currentFoodStopEating();
	}

    if (ch->currentDrinkIsDrinking())
	{
		if (player)
            player->sendLine(QObject::tr("&BSmetti di bere %1&d").arg(ch->currentDrink()));

		ch->currentDrinkStopDrinking();
	}

    if (player)
        static_cast<PcCharacter*>(ch)->sendLook();

    auto exitRoomCharacters = exitRoom->characters();

    if (ch->hasTemporaryStatus(QMUD::TemporaryStatusType::FLY))
    {
        if (moveStr == QObject::tr("alto"))
            msgOther = QObject::tr("%1 arriva volando dall'alto").arg(ch->name(Q_NULLPTR));
        else if (moveStr == QObject::tr("basso"))
            msgOther = QObject::tr("%1 arriva volando dal basso").arg(ch->name(Q_NULLPTR));
        else
            msgOther = QObject::tr("%1 arriva volando da %2").arg(ch->name(Q_NULLPTR)).arg(arriveStr);
    }
    else
    {
        if (moveStr == QObject::tr("alto"))
            msgOther = QObject::tr("%1 arriva dall'alto").arg(ch->name(Q_NULLPTR));
        else if (moveStr == QObject::tr("basso"))
            msgOther = QObject::tr("%1 arriva dal basso").arg(ch->name(Q_NULLPTR));
        else
            msgOther = QObject::tr("%1 arriva da %2").arg(ch->name(Q_NULLPTR)).arg(arriveStr);
    }

    for (Character* chInRoom : exitRoomCharacters)
    {
        if (chInRoom != ch && chInRoom->canSee(ch))
        {
            if (chInRoom->chFollowed() != ch->id())
            {
                if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
                {
                    Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

                    playerOther->sendLine(msgOther);

                    forceLook.insert(playerOther);
                }
                else if (chInRoom->chType() == Character::ChType::CHTYPE_NPC)
                {
                    NpcCharacter *npc = dynamic_cast<NpcCharacter*>(chInRoom);

                    npc->eventEnter(ch);
                }
            }
        }
    }

    // Character che seguono ch
    for (Character* chInRoom : characters)
    {
        if (chInRoom != ch && chInRoom->canSee(ch))
        {
            if (chInRoom->chFollowed() == ch->id())
            {
                Command::parse(command(), chInRoom)->execute();
            }
        }
    }

    if (room->isWilderness())
    {
        for (int x = -QMUD_WILD_DESCRIPTION_WIDTH_RADIUS; x <= QMUD_WILD_DESCRIPTION_WIDTH_RADIUS; ++x)
        {
            for (int y = -QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS; y <= QMUD_WILD_DESCRIPTION_HEIGHT_RADIUS; ++y)
            {
                int roomX = room->wildX() + x;
                int roomY = room->wildY() + y;

                Room* roomTmp = room->map()->room(roomX, roomY, room->wildMapIndex());

                if (roomTmp)
                {
                    auto chs = roomTmp->characters();
                    for (auto tmpCh : chs)
                    {
                        if (tmpCh->chType() == Character::ChType::CHTYPE_PC)
                        {
                            forceLook.insert(static_cast<PcCharacter*>(tmpCh)->player());
                        }
                    }
                }
            }
        }
    }

    for (auto p : forceLook)
        if (p->connection()->isCustomClient())
            p->pcCharacter()->sendLook();
}

void CommandMove::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}
