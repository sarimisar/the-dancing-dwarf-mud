/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandtakeoff.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"

using namespace CORE;

CommandTakeOff::CommandTakeOff() :
    Command()
{

}

CommandTakeOff::~CommandTakeOff()
{

}

bool CommandTakeOff::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("rimuovi"), cmd) ||
           Command::isOf(QObject::tr("rimuovo"), cmd);
}

QStringList CommandTakeOff::commands()
{
    return QStringList() << QObject::tr("rimuovo");
}

void CommandTakeOff::executePc(Character* ch, Player* player)
{
    if (args().isEmpty())
    {
        if (player)
            sendUsage(player);
        return;
    }

    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    // Controllo se posso agire
    QString message;
    if (!ch->canMakeAction(message))
    {
        if (player)
            player->sendLine(message);

        return;
    }

    if (args().size() == 1)
    {
        if (args()[0].toLower() == QObject::tr("tutto"))
        {
            auto equipment = ch->equipment();
            auto characters = ch->room()->characters();

            for (auto item : equipment)
                takeOffItem(ch, player, characters, item);
        }
        else
        {
            auto itemRemoved = ch->canUnequip(args()[0]);
            if (!itemRemoved)
            {
                if (player)
                    player->sendLine(QObject::tr("Non indossi %1").arg(args()[0]));
                return;
            }

            auto characters = ch->room()->characters();
            QString msgCh = QObject::tr("Smetti di usare %1").arg(itemRemoved->name());
            QString msgOther = QObject::tr("%1 smette di usare %2").arg(ch->name(Q_NULLPTR)).arg(itemRemoved->name());

            for (Character* chInRoom : characters)
            {
                if (chInRoom == ch)
                {
                    if (player)
                        player->sendLine(msgCh);
                }
                else if (chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(ch))
                {
                    Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
                    playerOther->sendLine(msgOther);
                }
            }

            ch->unequip(itemRemoved);
        }
    }
    else
    {
        if (player)
            sendUsage(player);
    }
}

void CommandTakeOff::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandTakeOff::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: prendo [<n>.]<nome>/tutto"));
}

bool CommandTakeOff::takeOffItem(Character* ch, Player *player, QList<Character*> characters, Item *targetItem)
{
    if (!ch->canUnequip(targetItem))
        return false;

    QString msgCh = QObject::tr("Smetti di usare %1").arg(targetItem->name());
    QString msgOther = QObject::tr("%1 smette di usare %2").arg(ch->name(Q_NULLPTR)).arg(targetItem->name());

    for (Character* chInRoom : characters)
    {
        if (chInRoom == ch)
        {
            if (player)
                player->sendLine(msgCh);
        }
        else if (chInRoom->chType() == Character::ChType::CHTYPE_PC && chInRoom->canSee(ch))
        {
            Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
            playerOther->sendLine(msgOther);
        }
    }

    ch->unequip(targetItem);

    return true;
}
