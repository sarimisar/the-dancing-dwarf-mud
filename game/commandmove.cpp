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
    return Command::isOf("moven", cmd) ||
            Command::isOf("moves", cmd) ||
            Command::isOf("movew", cmd) ||
            Command::isOf("movee", cmd) ||
            Command::isOf("moveu", cmd) ||
            Command::isOf("moved", cmd);
}

QStringList CommandMove::commands()
{
    return QStringList() << "moven"
                         << "moves"
                         << "movew"
                         << "movee"
                         << "moveu"
                         << "moved";
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
    RoomExit exit;

    if (QString("moven").startsWith(moveCmd))
    {
        exit = room->exitNorth();
    }
    else if (QString("moves").startsWith(moveCmd))
    {
        exit = room->exitSouth();
    }
    else if (QString("movew").startsWith(moveCmd))
    {
        exit = room->exitWest();
    }
    else if (QString("movee").startsWith(moveCmd))
    {
        exit = room->exitEast();
    }
    else if (QString("moveu").startsWith(moveCmd))
    {
        exit = room->exitUp();
    }
    else if (QString("moved").startsWith(moveCmd))
    {
        exit = room->exitDown();
    }

    if (!exit.isValid())
        return;

    if (!exit.isOpen())
        return;

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

    for (Character* chInRoom : characters)
    {
        if (chInRoom != ch && chInRoom->canSee(ch))
        {
            if (chInRoom->chType() == Character::ChType::CHTYPE_PC &&
                    chInRoom->chFollowed() != ch->id())
            {
                Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

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

    for (Character* chInRoom : exitRoomCharacters)
    {
        if (chInRoom != ch && chInRoom->canSee(ch))
        {
            if (chInRoom->chFollowed() != ch->id())
            {
                if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
                {
                    Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

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
