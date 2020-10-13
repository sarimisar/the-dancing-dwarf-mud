/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commanddrop.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"

using namespace CORE;

CommandDrop::CommandDrop() :
    Command()
{

}

CommandDrop::~CommandDrop()
{

}

bool CommandDrop::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("posa"), cmd) ||
           Command::isOf(QObject::tr("poso"), cmd);
}

QStringList CommandDrop::commands()
{
    return QStringList() << QObject::tr("poso");
}

void CommandDrop::executePc(Character* ch, Player* player)
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
            auto characters = ch->room()->characters();
            auto inventory = ch->inventory();

            if (inventory.isEmpty())
            {
                if (player)
                    player->sendLine(QObject::tr("Non hai nulla da posare nel tuo inventario"));
                return;
            }

            for (auto item : inventory)
                dropItem(ch, player, characters, item);
        }
        else
        {
            bool wrongUsage = false;
            Item* item = findItemInInventory(args()[0], ch->inventory(true), wrongUsage);

            if (wrongUsage)
            {
                if (player)
                    sendUsage(player);
                return;
            }

            if (!item)
            {
                if (player)
                    player->sendLine(QObject::tr("Non porti '%1' con te").arg(args()[0]));
                return;
            }

            if (ch->isEquipped(item))
            {
                if (player)
                    player->sendLine(QObject::tr("Devi prima rimuovere %1").arg(item->name()));
                return;
            }

            dropItem(ch, player, ch->room()->characters(), item);
        }
    }
    else
    {
        if (player)
            sendUsage(player);
    }
}

void CommandDrop::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandDrop::sendUsage(Player *player, bool help)
{
    if (help)
    {
        player->sendLine(QObject::tr("Permette di posare un oggetto del tuo inventario"));
    }

    player->sendLine(QObject::tr("Uso: posa [<n>.]<nome>/tutto"));
}

bool CommandDrop::dropItem(Character* ch, Player *player, QList<Character*> characters, Item *targetItem)
{
    if (targetItem->isBind())
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi posare %1, e' legato a te.").arg(targetItem->name()));
        return false;
    }

    QString msgCh = QObject::tr("Posi %1").arg(targetItem->name());
    QString msgOther = QObject::tr("%1 posa %2");

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
            playerOther->sendLine(msgOther.arg(ch->name(chInRoom)).arg(targetItem->name()));
        }
    }

    targetItem->setParent(ch->room());
    targetItem->setAutoDestroy();
    return true;
}
