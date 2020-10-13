/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandwear.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"
#include "itemarmor.h"
#include "itemweapon.h"
#include "itemtrinket.h"

using namespace CORE;

CommandWear::CommandWear(ActionType action) :
    Command(),
    m_eAction(action)
{

}

CommandWear::~CommandWear()
{

}

bool CommandWear::isOf(QString cmd, ActionType& action)
{
    if (Command::isOf(QObject::tr("indossa"), cmd) ||
        Command::isOf(QObject::tr("indosso"), cmd))
    {
        action = ActionType::WEAR;
        return true;
    }
    else if (Command::isOf(QObject::tr("impugna"), cmd) ||
           Command::isOf(QObject::tr("impugno"), cmd))
    {
        action = ActionType::HOLD;
        return true;
    }
    else if (Command::isOf(QObject::tr("afferra"), cmd) ||
           Command::isOf(QObject::tr("afferro"), cmd))
    {
        action = ActionType::GRAB;
        return true;
    }

    return false;
}

QStringList CommandWear::commands()
{
    return QStringList() << QObject::tr("indosso")
                         << QObject::tr("impugno")
                         << QObject::tr("afferro");
}

void CommandWear::executePc(Character* ch, Player* player)
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
                    player->sendLine(QObject::tr("Non porti nulla con te"));
                return;
            }

            for (auto item : inventory)
                wearItem(ch, player, characters, item);
        }
        else
        {
            bool wrongUsage = false;
            Item* item = findItemInInventory(args()[0], ch->inventory(), wrongUsage);

            if (wrongUsage)
            {
                if (player)
                    sendUsage(player);
                return;
            }

            if (!item)
            {
                if (player)
                    player->sendLine(QObject::tr("Non porti con te nessun '%1'").arg(args()[0]));
                return;
            }

            if (m_eAction == ActionType::HOLD && item->itemType() != QMUD::ItemType::WEAPON)
            {
                if (player)
                    player->sendLine(QObject::tr("Non puoi impugnare %1").arg(item->name()));
                return;
            }
            else if (m_eAction == ActionType::GRAB && item->itemType() != QMUD::ItemType::WEAPON)
            {
                if (player)
                    player->sendLine(QObject::tr("Non puoi afferrare %1").arg(item->name()));
                return;
            }

            wearItem(ch, player, ch->room()->characters(), item);
        }
    }
    else
    {
        if (player)
            sendUsage(player);
    }
}

void CommandWear::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandWear::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: indosso/impugno/afferro [<n>.]<nome>/tutto"));
}

bool CommandWear::wearItem(Character* ch, Player *player, QList<Character*> characters, Item *targetItem)
{
    QString msgCh;
    QString msgOther;

    if (targetItem->itemType() == QMUD::ItemType::WEAPON)
    {
        if (!ch->chClass()->itemClasses().contains(qobject_cast<ItemWeapon*>(targetItem)->classType()))
        {
            if (player)
                player->sendLine(QObject::tr("%1 non e' un oggetto adatto a te").arg(targetItem->name()));

            return false;
        }

        if (targetItem->level() > ch->level())
        {
            if (player)
                player->sendLine(QObject::tr("Non hai raggiunto il livello necessario per impugnare %1").arg(targetItem->name()));

            return false;
        }

        msgCh = QObject::tr("Impugni %1").arg(targetItem->name());
        msgOther = QObject::tr("%1 impugna %2").arg(ch->name(Q_NULLPTR)).arg(targetItem->name());

        QString alreadyWearItemName;

        if (!ch->canEquip(targetItem, alreadyWearItemName))
        {
            if (!alreadyWearItemName.isEmpty())
            {
                if (player)
                    player->sendLine(QObject::tr("Stai gia' impugnando %1").arg(alreadyWearItemName));
            }
            else
            {
                Q_ASSERT(false);

                if (player)
                    player->sendLine(QObject::tr("Non puoi impugnare %1").arg(targetItem->name()));
            }

            return false;
        }
    }
    else if (targetItem->itemType() == QMUD::ItemType::ARMOR ||
             targetItem->itemType() == QMUD::ItemType::TRINKET)
    {
        if ((targetItem->itemType() == QMUD::ItemType::ARMOR && !ch->chClass()->itemClasses().contains(qobject_cast<ItemArmor*>(targetItem)->classType())) ||
            (targetItem->itemType() == QMUD::ItemType::TRINKET && !ch->chClass()->itemClasses().contains(qobject_cast<ItemTrinket*>(targetItem)->classType())))
        {
            if (player)
                player->sendLine(QObject::tr("%1 non e' un oggetto adatto a te").arg(targetItem->name()));

            return false;
        }

        if (targetItem->level() > ch->level())
        {
            if (player)
                player->sendLine(QObject::tr("Non hai raggiunto il livello necessario per indossare %1").arg(targetItem->name()));

            return false;
        }

        msgCh = QObject::tr("Indossi %1").arg(targetItem->name());
        msgOther = QObject::tr("%1 indossa %2").arg(ch->name(Q_NULLPTR)).arg(targetItem->name());

        QString alreadyWearItemName;

        if (!ch->canEquip(targetItem, alreadyWearItemName))
        {
            if (!alreadyWearItemName.isEmpty())
            {
                if (player)
                    player->sendLine(QObject::tr("Stai gia' indossando %1").arg(alreadyWearItemName));
            }
            else
            {
                Q_ASSERT(false);

                if (player)
                    player->sendLine(QObject::tr("Non puoi indossare %1").arg(targetItem->name()));
            }

            return false;
        }
    }
    else
    {
        if (player)
            player->sendLine(QObject::tr("Non puoi indossare %1").arg(targetItem->name()));
        return false;
    }

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

    targetItem->setParent(ch);
    ch->equip(targetItem);

    return true;
}
