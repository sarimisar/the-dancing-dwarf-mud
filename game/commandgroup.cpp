/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandgroup.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"
#include "group.h"
#include "player.h"

#include "../global.h"

using namespace CORE;

CommandGroup::CommandGroup() :
    Command()
{

}

CommandGroup::~CommandGroup()
{

}

bool CommandGroup::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("gruppo"), cmd);
}

QStringList CommandGroup::commands()
{
    return QStringList() << QObject::tr("gruppo");
}

void CommandGroup::executePc(Character* ch, Player* player)
{
    PcCharacter *pc = static_cast<PcCharacter*>(ch);

    if (args().size() == 1)
    {
        if (args()[0].compare("esci", Qt::CaseInsensitive) == 0)
        {
            if (!pc->group())
            {
                player->sendLine(QObject::tr("Non fai parte di un gruppo"));
                return;
            }

            pc->group()->removeCharacter(pc);
        }
        else
        {
            if (pc->group() && pc->group()->leader() != pc)
            {
                if (ch->canSee(pc->group()->leader()))
                    player->sendLine(QObject::tr("Non sei tu il leader del gruppo ma %1").arg(pc->group()->leader()->name(Q_NULLPTR)));
                else
                    player->sendLine(QObject::tr("Non sei tu il leader del gruppo"));
                return;
            }

            auto players = QMUD::qmudWorld->playersByConnectionId();
            PcCharacter* pcToAdd = Q_NULLPTR;

            for (auto it = players.begin(); it != players.end(); ++it)
            {
                if (it.value()->pcCharacter())
                {
                    if (ch->canSee(it.value()->pcCharacter()) && it.value()->pcCharacter()->name(Q_NULLPTR).startsWith(args()[0], Qt::CaseInsensitive))
                    {
                        pcToAdd = it.value()->pcCharacter();
                        break;
                    }
                }
            }

            if (!pcToAdd)
            {
                player->sendLine(QObject::tr("Non c'e' %1 connesso").arg(args()[0]));
                return;
            }

            if (pcToAdd == ch)
            {
                player->sendLine(QObject::tr("Non puoi creare un gruppo con te stesso"));
                return;
            }

            if (pcToAdd->chFollowed() != ch->id())
            {
                player->sendLine(QObject::tr("%1 non ti sta seguendo").arg(pcToAdd->name(Q_NULLPTR)));
                return;
            }

            if (pcToAdd->room() != ch->room())
            {
                player->sendLine(QObject::tr("%1 non e' qui").arg(pcToAdd->name(Q_NULLPTR)));
                return;
            }

            if (pcToAdd->group())
            {
                if (pcToAdd->group() == pc->group())
                    player->sendLine(QObject::tr("%1 fa gia' parte del tuo gruppo").arg(pcToAdd->name(Q_NULLPTR)));
                else
                    player->sendLine(QObject::tr("%1 fa gia' parte di un gruppo").arg(pcToAdd->name(Q_NULLPTR)));
                return;
            }

            if (!pc->group())
            {
                Group* group = new Group(QMUD::qmudWorld);
                QMUD::qmudWorld->addGroup(group);
                group->addCharacter(pc);
                pc->setGroup(group);
            }

            pcToAdd->setGroup(pc->group());
            pc->group()->addCharacter(pcToAdd);
        }
    }
    else
    {
        if (!pc->group())
        {
            player->sendLine(QObject::tr("Non fai parte di un gruppo"));
            return;
        }

        auto chs = pc->group()->characters();

        player->sendLine("&b");

        for (auto c : chs)
        {
            if (c == pc->group()->leader())
                player->sendLine(c->name(ch) + " " + QObject::tr("&W(leader)&b"));
            else
                player->sendLine(c->name(ch));
        }

        player->send("&d");
    }
}

void CommandGroup::executeNpc(Character* ch)
{
    Q_UNUSED(ch);

    Q_ASSERT(false);
}

void CommandGroup::sendUsage(Player *player, bool help)
{
    if (player)
    {
        if (help)
        {
            player->sendLine(QObject::tr("Permette di creare e gestire un gruppo"));
        }

        player->sendLine(QObject::tr("Uso: gruppo      - Informazioni del tuo gruppo" QMUD_GLOBAL_NEW_LINE
                                     "     gruppo nome - Creare un gruppo o aggiungere qualcuno al gruppo" QMUD_GLOBAL_NEW_LINE
                                     "     gruppo esci - Uscire dal gruppo"));
    }
}
