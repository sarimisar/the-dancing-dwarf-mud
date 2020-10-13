/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandinventory.h"
#include "world.h"
#include "item.h"

using namespace CORE;

CommandInventory::CommandInventory(ActionType action) :
    Command(),
    m_eAction(action)
{

}

CommandInventory::~CommandInventory()
{

}

bool CommandInventory::isOf(QString cmd, ActionType &action)
{
    if (Command::isOf(QObject::tr("inventario"), cmd))
    {
        action = ActionType::INVENTORY;
        return true;
    }
    else if (cmd.toUpper() == QObject::tr("SACRIFICA") ||
             cmd.toUpper() == QObject::tr("SACRIFICO"))
    {
        action = ActionType::SACRIFY;
        return true;
    }

    return false;
}

QStringList CommandInventory::commands()
{
    return QStringList() << QObject::tr("inventario")
                         << QObject::tr("SACRIFICO");
}

void CommandInventory::executePc(Character *ch, Player* player)
{
    if (m_eAction == ActionType::INVENTORY)
    {
        auto inventory = ch->inventory();
        auto equipment = ch->equipment();

        // Oggetti
        QVector<Item*> items;
        QVector<int> itemsCounter;

        for (auto item : inventory)
        {
            bool found = false;

            for (int i=0; i<items.size(); ++i)
            {
                if (items[i]->staticId() == item->staticId() &&
                    items[i]->mapStaticId() == item->mapStaticId() &&
                    items[i]->IsModify() == item->IsModify() &&
                    items[i]->name() == item->name() &&
                    items[i]->level() == item->level())
                {
                    itemsCounter[i]++;
                    found = true;
                }
            }

            if (!found)
            {
                items.push_back(item);
                itemsCounter.push_back(1);
            }
        }

        if (items.isEmpty())
            player->sendLine(QObject::tr("Il tuo inventario e' vuoto"));
        else
        {
            for (int i=0; i<items.size(); ++i)
            {
                player->sendLine(QObject::tr("   [%[%1]&C&uINFO&d%[]] %3%2&d %4")
                                 .arg(QObject::tr("informazioni") + " " + QString::number(items[i]->id()))
                                 .arg(QMUD::capitalize(items[i]->name()))
                                 .arg(QMUD::itemQualityToColor(items[i]->quality()))
                                 .arg(itemsCounter[i] > 1 ? " [" + QString::number(itemsCounter[i]) + "]" : ""));
            }
        }

        player->sendLine(QObject::tr("&WStai trasportando %1/%2 oggetti&d").
                         arg(inventory.size() + equipment.size()).
                         arg(QMUD_GLOBAL_CH_MAX_NUMBER_OF_ITEMS_IN_INVENTORY));
    }
    else if (m_eAction == ActionType::SACRIFY)
    {
        if (args().size() != 1)
        {
            sendUsage(player);
            return;
        }

        if (ch->isUnconscious())
        {
            player->sendLine(QObject::tr("Sei incoscente."));
            return;
        }

        // Controllo se posso agire
        QString message;
        if (!ch->canMakeAction(message))
        {
            player->sendLine(message);
            return;
        }

        Item *item = Q_NULLPTR;

        // sacrifico <item id>
        QRegExp re("\\d*");
        if (re.exactMatch(args()[0]))
        {
            auto inventory = ch->inventory(true);
            auto id = args()[0].toULongLong();

            for (auto obj : inventory)
            {
                if (obj->id() == id)
                {
                    item = obj;
                    break;
                }
            }

            if (!item)
            {
                player->sendLine(QObject::tr("Non porti l'oggeto %1 con te").arg(args()[0]));
                return;
            }

            if (ch->isEquipped(item))
            {
                player->sendLine(QObject::tr("Devi prima rimuovere %1").arg(item->name()));
                return;
            }
        }

        player->sendConfirmationRequest(QObject::tr("SACRIFICO") + " " + args()[0]);
        static_cast<PcCharacter*>(ch)->sendRequest(QObject::tr("Vuoi sacrificare %1 [%2]?\nATTENZIONE! Non potrai tornare indietro!").arg(item->name())
                                                   .arg(QMUD::itemQualityToReadableString(item->quality(), false).toUpper()));
    }
}

void CommandInventory::executePcConfirmation(Character* ch, Player* player)
{
    if (m_eAction == ActionType::SACRIFY)
    {
        if (args().size() != 1)
        {
            sendUsage(player);
            return;
        }

        if (ch->isUnconscious())
        {
            player->sendLine(QObject::tr("Sei incoscente."));
            return;
        }

        // Controllo se posso agire
        QString message;
        if (!ch->canMakeAction(message))
        {
            player->sendLine(message);
            return;
        }

        Item *item = Q_NULLPTR;

        // sacrifico <item id>
        QRegExp re("\\d*");
        if (re.exactMatch(args()[0]))
        {
            auto inventory = ch->inventory(true);
            auto id = args()[0].toULongLong();

            for (auto obj : inventory)
            {
                if (obj->id() == id)
                {
                    item = obj;
                    break;
                }
            }

            if (!item)
            {
                player->sendLine(QObject::tr("Non porti l'oggeto %1 con te").arg(args()[0]));
                return;
            }

            if (ch->isEquipped(item))
            {
                player->sendLine(QObject::tr("Devi prima rimuovere %1").arg(item->name()));
                return;
            }

            player->sendLine(QObject::tr("Sacrifichi %1 agli dei").arg(item->name()));
            item->destroy();
        }
    }
}

void CommandInventory::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandInventory::sendUsage(Player *player, bool help)
{
    if (help)
    {
        player->sendLine(QObject::tr("Mostra l'elenco degli oggetti del tuo inventario"));
    }

    player->sendLine(QObject::tr("Uso: inventario"));
}
