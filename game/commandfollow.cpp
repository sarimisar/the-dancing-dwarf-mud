/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandfollow.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"

using namespace CORE;

CommandFollow::CommandFollow() :
    Command()
{

}

CommandFollow::~CommandFollow()
{

}

bool CommandFollow::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("segui"), cmd) ||
           Command::isOf(QObject::tr("seguo"), cmd);
}

QStringList CommandFollow::commands()
{
    return QStringList() << QObject::tr("seguo");
}

void CommandFollow::executePc(Character* ch, Player* player)
{
    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    if (args().size() == 1)
    {
        if (args()[0].toLower() == "me")
        {
            if (ch->chFollowed() != QMUD::IdInvalid)
            {
                if (player)
                    player->sendLine(QObject::tr("Smetti di seguire %1").arg(ch->chFollowedName()));

                ch->setChFollowed(QMUD::IdInvalid, QString());

                return;
            }
            else
            {
                if (player)
                    player->sendLine(QObject::tr("Non stai seguendo nessuno"));
                return;
            }
        }

        bool wrongUsage = false;
        Entity* targetEntity = findTargetInRoom(args()[0], wrongUsage, true);

        if (wrongUsage)
        {
            if (player)
                sendUsage(player);
            return;
        }

        if (!targetEntity)
        {
            if (player)
                player->sendLine(QObject::tr("%1 non e' qui").arg(args()[0]));
            return;
        }

        Character *targetCh = qobject_cast<Character*>(targetEntity);

        if (!targetCh)
        {
            if (player)
                player->sendLine(QObject::tr("Non puoi seguire %1").arg(targetEntity->name(Q_NULLPTR)));
            return;
        }

        if (targetCh->chFollowed() == ch->id())
        {
            if (player)
                player->sendLine(QObject::tr("Non puoi seguire %1, sta seguendo te").arg(targetCh->name(Q_NULLPTR)));
            return;
        }

        if (ch->chFollowed() == targetCh->id())
        {
            if (player)
                player->sendLine(QObject::tr("Stai gia' seguendo %1").arg(targetCh->name(Q_NULLPTR)));
            return;
        }

        Room* room = ch->room();


        auto characters = room->characters();

        QString msgCh;
        QString msgTarget;
        QString msgOther;

        if (ch == targetEntity)
        {
            if (ch->chFollowed() != QMUD::IdInvalid)
            {
                if (player)
                    player->sendLine(QObject::tr("Smetti di seguire %1").arg(ch->chFollowedName()));

                ch->setChFollowed(QMUD::IdInvalid, QString());

                return;
            }
            else
            {
                if (player)
                    player->sendLine(QObject::tr("Segui te stesso"));
                return;
            }
        }
        else
        {
            msgCh = QObject::tr("Inizi a seguire %1").arg(targetCh->name(ch));;
            msgTarget = QObject::tr("%1 inizia a seguirti").arg(ch->name(targetCh));
            msgOther = QObject::tr("%1 inizia a seguire %2").arg(ch->name(Q_NULLPTR)).arg(targetCh->name(Q_NULLPTR));
        }

        for (Character* chInRoom : characters)
        {
            if (chInRoom == ch)
            {
                if (player)
                    player->sendLine(msgCh);
            }
            else if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
            {
                Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

                if (targetCh == chInRoom)
                    playerOther->sendLine(msgTarget);
                else if (chInRoom->canSee(ch) && chInRoom->canSee(targetCh))
                    playerOther->sendLine(msgOther);
            }
        }

        ch->setChFollowed(targetCh->id(), targetCh->name(Q_NULLPTR));
    }
    else
    {
        if (player)
            sendUsage(player);
    }
}

void CommandFollow::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandFollow::sendUsage(Player *player, bool help)
{
    if (player)
    {
        if (help)
        {
            player->sendLine(QObject::tr("Permette di seguire un altro giocatore"));
        }

        player->sendLine(QObject::tr("Uso: segui [<n>.]<nome>"));
    }
}
