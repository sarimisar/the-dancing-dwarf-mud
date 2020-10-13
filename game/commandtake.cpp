/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandtake.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"

using namespace CORE;

CommandTake::CommandTake() :
    Command()
{

}

CommandTake::~CommandTake()
{

}

bool CommandTake::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("prendi"), cmd) ||
           Command::isOf(QObject::tr("prendo"), cmd);
}

QStringList CommandTake::commands()
{
    return QStringList() << QObject::tr("prendo");
}

void CommandTake::executePc(Character* ch, Player* player)
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
    if (ch->canMakeAction(message))
    {
        if (player)
            player->sendLine(message);
    }

    if (args().size() == 1)
    {
        if (args()[0].toLower() == QObject::tr("tutto"))
        {
            bool foundObj = false;

            auto characters = ch->room()->characters();
            QList<Entity*> entities = ch->room()->entities(ch);

            for (auto it = entities.begin(); it != entities.end(); ++it)
            {
                if ((*it)->entityType() == Entity::EntityType::ITEM)
                {
                    foundObj = true;
                    takeItem(ch, player, characters, static_cast<Item*>(static_cast<Item*>(*it)));
                }
            }

            if (!foundObj)
            {
                if (player)
                    player->sendLine(QObject::tr("Non c'e' nulla da prendere qui"));
                return;
            }
        }
        else
        {
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
                    player->sendLine(QObject::tr("'%1' non e' qui").arg(args()[0]));
                return;
            }

            if (targetEntity->entityType() != Entity::EntityType::ITEM)
            {
                if (player)
                    player->sendLine(QObject::tr("Non puoi prendere %1").arg(targetEntity->name(Q_NULLPTR)));
                return;
            }

            takeItem(ch, player, ch->room()->characters(), static_cast<Item*>(targetEntity));
        }
    }
    else
    {
        if (player)
            sendUsage(player);
    }
}

void CommandTake::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandTake::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: prendo [<n>.]<nome>/tutto"));
}

bool CommandTake::takeItem(Character* ch, Player *player, QList<Character*> characters, Item *targetItem)
{
    if (targetItem->itemType() != QMUD::ItemType::BASE &&
        targetItem->itemType() != QMUD::ItemType::WEAPON &&
        targetItem->itemType() != QMUD::ItemType::ARMOR &&
        targetItem->itemType() != QMUD::ItemType::TRINKET &&
        targetItem->itemType() != QMUD::ItemType::FOOD &&
        targetItem->itemType() != QMUD::ItemType::DRINK &&
        targetItem->itemType() != QMUD::ItemType::POTION &&
        targetItem->itemType() != QMUD::ItemType::TONIC)
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi prendere %1").arg(targetItem->name()));
        return false;
    }

    if (targetItem->isBind())
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi prendere %1, e' legato ad un altro.").arg(targetItem->name()));
        return false;
    }

    if (ch->inventorySize(true) >= QMUD_GLOBAL_CH_MAX_NUMBER_OF_ITEMS_IN_INVENTORY)
    {
        if (player)
            player->sendLine(QObject::tr("Stai trasportando troppi oggetti."));
        return false;
    }

    QString msgCh = QObject::tr("Prendi %1").arg(targetItem->name());
    QString msgOther = QObject::tr("%1 prende %2");

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

    targetItem->setParent(ch);
    targetItem->setAutoDestroy(-1);
    return true;
}
