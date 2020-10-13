/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandshop.h"
#include "world.h"
#include "room.h"
#include "item.h"
#include "npccharacter.h"
#include "globaldebug.h"

using namespace CORE;

CommandShop::CommandShop(ActionType action) :
    Command(),
    m_eActionType(action)
{

}

CommandShop::~CommandShop()
{

}

bool CommandShop::isOf(QString cmd, ActionType &action)
{
    if (Command::isOf(QObject::tr("negozio"), cmd) ||
        Command::isOf(QObject::tr("lista"), cmd))
    {
        action = ActionType::LIST;
        return true;
    }
    else if (Command::isOf(QObject::tr("compra"), cmd) ||
             Command::isOf(QObject::tr("compro"), cmd))
    {
        action = ActionType::BUY;
        return true;
    }
    else
        return false;
}

QStringList CommandShop::commands()
{
    return QStringList() << QObject::tr("negozio")
                         << QObject::tr("lista")
                         << QObject::tr("compro");
}

void CommandShop::executePc(Character *ch, Player* player)
{
    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    if (m_eActionType == ActionType::LIST)
    {
        bool isEmpty = true;
        auto characters = ch->room()->characters();
        int objIndex = 1;

        for (auto it = characters.begin(); it != characters.end(); ++it)
        {
            if ((*it)->chType() == Character::ChType::CHTYPE_NPC)
            {
                NpcCharacter *npc = static_cast<NpcCharacter*>(*it);

                if (ch->canSee(npc) && npc->isVendor())
                {
                    auto list = npc->shopItems();

                    if (!list.isEmpty())
                    {
                        isEmpty = false;

                        player->sendLine(QObject::tr("%1 vende:").arg(npc->name(Q_NULLPTR)));

                        for (auto itemData : list)
                        {
                            auto map = QMUD::qmudWorld->map(itemData.mapId);

                            if (map)
                            {
                                auto itemList = map->mapJsonObjItemsById();
                                auto itItemJson = itemList.find(itemData.id);

                                if (itItemJson != itemList.end())
                                {
                                    isEmpty = false;

                                    qint64 coins = Item::jsonToSellValue(itItemJson.value());
                                    QString name = Item::jsonToName(itItemJson.value());
                                    player->sendLine(QString("  %1 - %2 %3")
                                                     .arg(objIndex++, 2)
                                                     .arg(name.leftJustified(40, '.', true))
                                                     .arg(QMUD::coinsToReadableString(qRound64(coins * npc->shopSaleInflation()))));
                                }
                            }
                        }
                    }
                }
            }
        }

        if (isEmpty)
        {
            player->sendLine(QObject::tr("Non c'e' nessun negoziante qui"));
        }
    }
    else if (m_eActionType == ActionType::BUY)
    {
        if (args().size() != 1)
        {
            sendUsage(player);
            return;
        }

        bool buyByIndex = false;

        QRegExp re("\\d*");
        if (re.exactMatch(args()[0]))
        {
            buyByIndex = true;
        }

        QString objNameToBuy = args()[0];
        bool isEmpty = true;
        auto characters = ch->room()->characters();
        int objIndex = 1;

        for (auto it = characters.begin(); it != characters.end(); ++it)
        {
            if ((*it)->chType() == Character::ChType::CHTYPE_NPC)
            {
                NpcCharacter *npc = static_cast<NpcCharacter*>(*it);

                if (ch->canSee(npc) && npc->isVendor())
                {
                    auto list = npc->shopItems();

                    if (!list.isEmpty())
                    {
                        isEmpty = false;

                        for (auto itemData : list)
                        {
                            if (!buyByIndex || objIndex == objNameToBuy.toInt())
                            {
                                auto map = QMUD::qmudWorld->map(itemData.mapId);

                                if (map)
                                {
                                    auto itemList = map->mapJsonObjItemsById();
                                    auto itItemJson = itemList.find(itemData.id);

                                    if (itItemJson != itemList.end())
                                    {
                                        QString itemName = Item::jsonToName(itItemJson.value());
                                        if (!buyByIndex && !itemName.contains(objNameToBuy))
                                        {
                                            objIndex++;
                                            continue;
                                        }

                                        isEmpty = false;
                                        qint64 coins = qRound64(Item::jsonToSellValue(itItemJson.value()) * npc->shopSaleInflation());
                                        QString name = Item::jsonToName(itItemJson.value());
                                        int level = Item::jsonToLevel(itItemJson.value());

                                        if (ch->coins() < coins)
                                        {
                                            player->sendLine(QObject::tr("Non hai a disposizione %1 per comprare %2")
                                                             .arg(QMUD::coinsToReadableString(coins))
                                                             .arg(name));
                                            return;
                                        }

                                        if (ch->level() < level)
                                        {
                                            player->sendLine(QObject::tr("Non hai il livello adatto per comprare %1").arg(name));
                                            return;
                                        }

                                        if (ch->inventorySize(true) >= QMUD_GLOBAL_CH_MAX_NUMBER_OF_ITEMS_IN_INVENTORY)
                                        {
                                            if (player)
                                                player->sendLine(QObject::tr("Stai trasportando troppi oggetti."));
                                            return;
                                        }

                                        Item* item = Item::loadItem(itItemJson.value(), ch, itemData.mapId);

                                        if (item)
                                        {
                                            if (!ch->removeCoins(coins))
                                            {
                                                player->sendLine(QObject::tr("Non hai a disposizione %1 per comprare %2")
                                                                 .arg(QMUD::coinsToReadableString(coins))
                                                                 .arg(name));
                                                item->destroy();
                                                return;
                                            }

                                            player->sendLine(QObject::tr("Compri %1 per %2")
                                                             .arg(item->name())
                                                             .arg(QMUD::coinsToReadableString(coins)));
                                            return;
                                        }
                                    }
                                }

                                player->sendLine(QObject::tr("Qualcosa e' andato storto, contatta un amministratore"));
                                GLOBAL_WARNING(QObject::tr("Errore nell'acquisto dell'oggetto %1:%2 dell'NPC %3:%4:%5")
                                              .arg(itemData.mapId)
                                              .arg(itemData.id)
                                              .arg(npc->mapStaticId())
                                              .arg(npc->staticId())
                                              .arg(npc->name(Q_NULLPTR)));

                                return;
                            }

                            objIndex++;
                        }
                    }
                }
            }
        }

        if (isEmpty)
        {
            player->sendLine(QObject::tr("Non c'e' nessun negoziante qui"));
        }
        else
        {
            if (buyByIndex)
                player->sendLine(QObject::tr("L'oggetto numero %1 non e' presente nella lista").arg(objNameToBuy.toInt()));
            else
                player->sendLine(QObject::tr("Non c'e' nessun %1 da comprare").arg(objNameToBuy));
        }
    }
}

void CommandShop::executeNpc(Character* ch)
{
    Q_UNUSED(ch)

    Q_ASSERT(false);
}

void CommandShop::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: negozio         - Mostra la lista degli articoli in vendita" QMUD_GLOBAL_NEW_LINE
                                 "     compro <indice> - Acquista l'oggetto specificato"));
}
