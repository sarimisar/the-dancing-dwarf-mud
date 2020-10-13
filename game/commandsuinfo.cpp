/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandsuinfo.h"
#include "world.h"
#include "room.h"
#include "item.h"
#include "itemarmor.h"
#include "itemweapon.h"
#include "itemtrinket.h"
#include "itemfood.h"
#include "itemdrink.h"
#include "itempotion.h"
#include "itemresource.h"
#include "itemtonic.h"
#include "npccharacter.h"
#include "pccharacter.h"

#include "../global.h"

using namespace CORE;

CommandSuInfo::CommandSuInfo() :
    Command()
{

}

CommandSuInfo::~CommandSuInfo()
{

}

bool CommandSuInfo::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("suinfo"), cmd);
}

QStringList CommandSuInfo::commands()
{
    return QStringList() << QObject::tr("suinfo");
}

void CommandSuInfo::executePc(Character *ch, Player* player)
{
    if (args().isEmpty())
    {
        sendUsage(player);
        return;
    }

    if (QString("ch").startsWith(args()[0], Qt::CaseInsensitive))
    {
        if (args().size() == 2)
        {
            // Ch by id
            QRegExp re("\\d*");
            if (re.exactMatch(args()[1]))
            {
                auto chId = static_cast<QMUD::IdType>(args()[1].toULongLong());
                auto chTarget = QMUD::qmudWorld->character(chId);

                if (!chTarget)
                {
                    player->sendLine(QObject::tr("Character %1 non trovato").arg(chId));
                    return;
                }

                sendChInfo(chTarget, player);
            }
            // Ch by name
            else
            {
                bool wrongUsage = false;

                auto chTarget = findCharacterInMap(args()[1], wrongUsage, false);

                if (wrongUsage)
                {
                    sendUsage(player);
                    return;
                }

                if (chTarget)
                    sendChInfo(chTarget, player);
                else
                {
                    auto characters = QMUD::qmudWorld->characters();

                    for (auto c : characters)
                    {
                        if (c->name(Q_NULLPTR).contains(args()[1], Qt::CaseInsensitive))
                        {
                            sendChInfo(c, player);
                            return;
                        }
                    }

                    player->sendLine(QObject::tr("Character %1 non trovato").arg(args()[1]));
                    return;
                }
            }
        }
        else
        {
            sendUsage(player);
            return;
        }
    }
    else if (QString("chlist").startsWith(args()[0], Qt::CaseInsensitive))
    {
        // suinfo chlist
        if (args().size() == 1)
        {
            sendChList(player);
        }
        // suinfo chlist <nome>
        // suinfo chlist <first index>
        else if (args().size() == 2)
        {
            // ch by id
            QRegExp re("\\d*");
            if (re.exactMatch(args()[1]))
            {
                auto chId = static_cast<QMUD::IdType>(args()[1].toULongLong());
                sendChList(player, "", chId);
            }
            else
            {
                sendChList(player, args()[1]);
            }
        }
        // suinfo chlist <nome> <first index>
        else if (args().size() == 3)
        {
            QRegExp re("\\d*");
            if (re.exactMatch(args()[2]))
            {
                auto chId = static_cast<QMUD::IdType>(args()[2].toULongLong());
                sendChList(player, args()[1], chId);
            }
            else
            {
                sendUsage(player);
                return;
            }
        }
        else
        {
            sendUsage(player);
            return;
        }
    }
    else if (QString("item").startsWith(args()[0], Qt::CaseInsensitive))
    {
        if (args().size() == 2)
        {
            // Item by id
            QRegExp re("\\d*");
            if (re.exactMatch(args()[1]))
            {
                auto itemId = static_cast<QMUD::IdType>(args()[1].toULongLong());
                auto items = QMUD::qmudWorld->items();

                auto it = items.find(itemId);

                if (it == items.end())
                {
                    player->sendLine(QObject::tr("Item %1 non trovato").arg(itemId));
                    return;
                }

                sendItemInfo(it.value(), player);
            }
            // Item by name
            else
            {
                bool wrongUsage = false;
                auto item = findItemInInventory(args()[1], ch->inventory(true), wrongUsage);

                if (wrongUsage)
                {
                    sendUsage(player);
                    return;
                }

                if (item)
                    sendItemInfo(item, player);
                else
                {
                    player->sendLine(QObject::tr("Item %1 non trovato").arg(args()[1]));
                    return;
                }
            }
        }
        else
        {
            sendUsage(player);
            return;
        }
    }
    else if (QString("itemlist").startsWith(args()[0], Qt::CaseInsensitive))
    {
        // suinfo itemlist
        if (args().size() == 1)
        {
            sendItemList(player);
        }
        // suinfo itemlist <nome>
        // suinfo itemlist <first index>
        else if (args().size() == 2)
        {
            // Item by id
            QRegExp re("\\d*");
            if (re.exactMatch(args()[1]))
            {
                auto itemId = static_cast<QMUD::IdType>(args()[1].toULongLong());
                sendItemList(player, "", itemId);
            }
            else
            {
                sendItemList(player, args()[1]);
            }
        }
        // suinfo itemlist <nome> <first index>
        else if (args().size() == 3)
        {
            QRegExp re("\\d*");
            if (re.exactMatch(args()[2]))
            {
                auto itemId = static_cast<QMUD::IdType>(args()[2].toULongLong());
                sendItemList(player, args()[1], itemId);
            }
            else
            {
                sendUsage(player);
                return;
            }
        }
        else
        {
            sendUsage(player);
            return;
        }
    }
    else if (QString("room").startsWith(args()[0], Qt::CaseInsensitive))
    {
        if (args().size() == 2)
        {
            QRegExp re("\\d*");

            if (re.exactMatch(args()[1]))
            {
                auto roomId = static_cast<QMUD::StaticIdType>(args()[1].toULong());
                auto rooms = ch->room()->map()->rooms();
                auto it = rooms.find(roomId);

                if (it != rooms.end())
                {
                    sendRoomInfo(it.value(), player);
                }
                else
                {
                    player->sendLine(QObject::tr("Stanza %1 non trovata nella mappa corrente").arg(roomId));
                    return;
                }
            }
            else
            {
                sendUsage(player);
                return;
            }
        }
        else
        {
            sendRoomInfo(ch->room(), player);
        }
    }
    else if (QString("map").startsWith(args()[0], Qt::CaseInsensitive))
    {
        if (args().size() == 2)
        {
            QRegExp re("\\d*");

            if (QString("list").startsWith(args()[1], Qt::CaseInsensitive))
            {
                auto maps = QMUD::qmudWorld->maps();
                for (auto map : maps)
                {
                    player->sendLine(QString(" %1 - %2 [id:%3, rooms:%4]")
                                     .arg(map->staticId(), 6)
                                     .arg(map->name())
                                     .arg(map->id())
                                     .arg(map->rooms().size()));
                }
            }
            else if (re.exactMatch(args()[1]))
            {
                auto itemId = static_cast<QMUD::StaticIdType>(args()[1].toULong());
                auto maps = QMUD::qmudWorld->maps();
                auto it = maps.find(itemId);

                if (it != maps.end())
                {
                    sendMapInfo(it.value(), player);
                }
                else
                {
                    player->sendLine(QObject::tr("Mappa %1 non trovata").arg(itemId));
                    return;
                }
            }
            else
            {
                sendUsage(player);
                return;
            }
        }
        else
        {
            sendMapInfo(ch->room()->map(), player);
        }
    }
    else
    {
        sendUsage(player);
    }
}

void CommandSuInfo::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandSuInfo::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: suinfo ch [<n>.]<nome>" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo chlist <nome>" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo chlist <first index>" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo chlist <nome> <first index>" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo itemlist <nome>" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo itemlist <first index>" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo itemlist <nome> <first index>" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo item [<n>.]<nome>" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo room [list]" QMUD_GLOBAL_NEW_LINE
                                 "     suinfo map [list]" QMUD_GLOBAL_NEW_LINE));
}

void CommandSuInfo::sendChList(Player *player, QString filter, QMUD::IdType firstId)
{
    int counter = 0;
    auto characters = QMUD::qmudWorld->characters();

    for (auto it = characters.begin(); it != characters.end(); ++it)
    {
        if (it.key() >= firstId)
        {
            if (it.value()->name(Q_NULLPTR).contains(filter, Qt::CaseInsensitive))
            {
                if (it.value()->chType() == Character::ChType::CHTYPE_NPC)
                {
                    NpcCharacter* npc = static_cast<NpcCharacter*>(it.value());

                    player->sendLine(QString(" %1 - %2 [MAP:%3] [ROOM:%4:%5]")
                                     .arg(it.key(), 6)
                                     .arg(QString(npc->name(Q_NULLPTR)).leftJustified(40, '.', true))
                                     .arg(npc->mapStaticId())
                                     .arg(it.value()->parent() ? static_cast<Entity*>(it.value()->parent())->name(Q_NULLPTR) : "NULL")
                                     .arg(it.value()->parent() ? static_cast<Entity*>(it.value()->parent())->id() : 0));
                }
                else
                {
                    PcCharacter* pc = static_cast<PcCharacter*>(it.value());

                    player->sendLine(QString(" %1 - %2 [ROOM:%3:%4]")
                                     .arg(it.key(), 6)
                                     .arg(QString(pc->name(Q_NULLPTR)).leftJustified(40, '.', true))
                                     .arg(it.value()->parent() ? static_cast<Entity*>(it.value()->parent())->name(Q_NULLPTR) : "NULL")
                                     .arg(it.value()->parent() ? static_cast<Entity*>(it.value()->parent())->id() : 0));
                }

                counter++;

                if (counter >= 50)
                    break;
            }
        }
    }

    if (counter == 0)
    {
        player->sendLine(QObject::tr("Nessun oggetto trovato"));
    }
}

void CommandSuInfo::sendChInfo(Character *ch, Player *player)
{
    player->sendLine(QString("&W        Name&d: %1").arg(ch->name(Q_NULLPTR)));
    player->sendLine(QString("&W        Room&d: %1:%2").arg(ch->room()->name()).arg(ch->room()->id()));
    player->sendLine(QString("&W       Class&d: %1").arg(ch->chClass()->toReadableString()));
    player->sendLine(QString("&W       Level&d: %1").arg(ch->level()));
    player->sendLine(QString("&W        Race&d: %1").arg(QMUD::Race::toReadableString(ch->race())));
    player->sendLine(QString("&W          HP&d: %1/%2").arg(ch->hitPointsCurrent()).arg(ch->hitPointsCurrentMaximum()));
    player->sendLine(QString("&W          MP&d: %1/%2").arg(ch->manaPointsCurrent()).arg(ch->manaPointsCurrentMaximum()));
    player->sendLine(QString("&W          AP&d: %1/%2").arg(ch->actionPointsCurrent()).arg(ch->actionPointsCurrentMaximum()));
    player->sendLine(QString("&W         COS&d: %1 (base %2), TS %3, CD %4").arg(ch->constitution()).arg(ch->constitutionBase()).arg(ch->tsConstitution()).arg(ch->cdConstitution()));
    player->sendLine(QString("&W         DEX&d: %1 (base %2), TS %3, CD %4").arg(ch->dexterity()).arg(ch->dexterityBase()).arg(ch->tsDexterity()).arg(ch->cdDexterity()));
    player->sendLine(QString("&W         INT&d: %1 (base %2), TS %3, CD %4").arg(ch->intelligence()).arg(ch->intelligenceBase()).arg(ch->tsIntelligence()).arg(ch->cdIntelligence()));
    player->sendLine(QString("&W         WIS&d: %1 (base %2), TS %3, CD %4").arg(ch->wisdom()).arg(ch->wisdomBase()).arg(ch->tsWisdom()).arg(ch->cdWisdom()));
    player->sendLine(QString("&W         CHA&d: %1 (base %2), TS %3, CD %4").arg(ch->charisma()).arg(ch->charismaBase()).arg(ch->tsCharisma()).arg(ch->cdCharisma()));
    player->sendLine(QString("&W         STR&d: %1 (base %2), TS %3, CD %4").arg(ch->strength()).arg(ch->strengthBase()).arg(ch->tsStrength()).arg(ch->cdStrength()));
    player->sendLine(QString("&W     HitDice&d: %1").arg(ch->hitDice()));
    player->sendLine(QString("&W    ManaDice&d: %1").arg(ch->manaDice()));
    player->sendLine(QString("&W          AC&d: %1").arg(ch->armorClass()));
    player->sendLine(QString("&W    HitBonus&d: %1").arg(ch->hitBonus()));
    player->sendLine(QString("&W DamageBonus&d: %1").arg(ch->damageBonus()));
    player->sendLine(QString("&W     Attacks&d: %1").arg(ch->numberOfAttacks()));
    player->sendLine(QString("&W      Follow&d: %1").arg(ch->chFollowedName()));

    auto targetCh = ch->targetCharacter(false);
    if (targetCh)
        player->sendLine(QString("&W      Target&d: %1 (%2)").arg(targetCh->name(Q_NULLPTR)).arg(targetCh->id()));

    QStringList status;
    auto tmpSts = ch->temporaryStatus();

    for (auto it = tmpSts.begin(); it != tmpSts.end(); ++it)
        status.push_back(QMUD::statusToReadableString(it.key()) + QString(" (%1)").arg(it.value()));

    player->sendLine(QString("&W      Status&d: %1").arg(status.join(", ")));
    player->sendLine(QString("&W       Coins&d: %1").arg(QMUD::coinsToReadableString(ch->coins())));

    if (ch->chType() == Character::ChType::CHTYPE_NPC)
    {
        NpcCharacter* npc = static_cast<NpcCharacter*>(ch);

        player->sendLine(QString("&W    StaticId&d: %1").arg(npc->staticId()));
        player->sendLine(QString("&W MapStaticId&d: %1").arg(npc->mapStaticId()));
        player->sendLine(QString("&WRoomStaticId&d: %1").arg(npc->roomStaticId()));
        player->sendLine(QString("&W     NpcType&d: %1").arg(QMUD::npcTypeToString(npc->npcType())));
        player->sendLine(QString("&W Description&d: %1").arg(npc->description()));
        player->sendLine(QString("&W     GiveExp&d: %1").arg(npc->giveExperience()));
        player->sendLine(QString("&W ToResurrect&d: %1").arg(npc->isToResurrect()));
        player->sendLine(QString("&W   IsHunting&d: %1").arg(npc->isHunting() ? "YES" : "NO"));

        auto loot = npc->loot();
        for (auto item : loot)
            player->sendLine(QString("&W        Loot&d: %1 [%2:%3] Id: %4, MapId: %5, Value: %6")
                             .arg(item.name())
                             .arg(QMUD::itemQualityToString(item.quality()))
                             .arg(QMUD::itemRarityToString(item.rarity()))
                             .arg(item.staticId())
                             .arg(item.mapStaticId())
                             .arg(item.coinsValue()));

        auto flags = npc->flags();
        QStringList strFlags;
        for (auto flag : flags)
            strFlags.push_back(QMUD::npcCharacterFlagTypeToString(flag));

        player->sendLine(QString("&W       Flags&d: %1&d").arg(strFlags.join(" ")));

        if (npc->isVendor())
        {
            player->sendLine(QString("&W    ShopBase&d: %1").arg(npc->shopPurchaseBase()));
            player->sendLine(QString("&W   ShopArmor&d: %1").arg(npc->shopPurchaseArmor()));
            player->sendLine(QString("&W  ShopWeapon&d: %1").arg(npc->shopPurchaseWeapon()));
            player->sendLine(QString("&W    ShopSale&d: %1").arg(npc->shopSaleInflation()));
            player->sendLine(QString("&WShopPurchase&d: %1").arg(npc->shopPurchaseInflation()));

            auto items = npc->shopItems();
            for (auto itemData : items)
            {
                auto map = QMUD::qmudWorld->map(itemData.mapId);

                if (map)
                {
                    auto itemList = map->mapJsonObjItemsById();
                    auto itItemJson = itemList.find(itemData.id);

                    if (itItemJson != itemList.end())
                    {
                        qint64 coins = Item::jsonToSellValue(itItemJson.value());
                        QString name = Item::jsonToName(itItemJson.value());
                        player->sendLine(QString("&W    ShopItem&d: %1 - %2").arg(name).arg(QMUD::coinsToReadableString(coins)));
                    }
                }
            }
        }

        auto scriptsMap = npc->scripts();

        for (auto it = scriptsMap.begin(); it != scriptsMap.end(); ++it)
        {
            player->sendLine(QString("&W----- Event %1 -----&d").arg(QMUD::eventTypeToString(it.key())));
            player->sendLine(it.value());
        }
    }
    else if (ch->chType() == Character::ChType::CHTYPE_PC)
    {
        PcCharacter* pc = static_cast<PcCharacter*>(ch);

        player->sendLine(QString("&W      Player&d: %1").arg(pc->player()->usernameBeautify()));
        player->sendLine(QString("&W  Connection&d: %1").arg(pc->connection()->address()));
        player->sendLine(QString("&W       Group&d: %1").arg(pc->group() ? "YES" : "NO"));

        auto loot = pc->loot();
        for (auto item : loot)
            player->sendLine(QString("&W        Loot&d: %1 [%2:%3] Id: %4, MapId: %5, Value: %6")
                             .arg(item.name())
                             .arg(QMUD::itemQualityToString(item.quality()))
                             .arg(QMUD::itemRarityToString(item.rarity()))
                             .arg(item.staticId())
                             .arg(item.mapStaticId())
                             .arg(item.coinsValue()));
    }

    auto inventory = ch->inventory(true);
    for (auto item : inventory)
        player->sendLine(QString("&W   Inventory&d: %1 [%2] Id: %3, MapId: %4, Value: %5").arg(item->name()).arg(QMUD::itemQualityToString(item->quality())).arg(item->staticId()).arg(item->mapStaticId()).arg(item->coinsValue()));

    auto resources = ch->resources();
    for (auto it = resources.begin(); it != resources.end(); ++it)
    {
        if (it.key() != QMUD::ResourceType::COINS)
        {
            QString strResource = QMUD::resourceToReadableString(it.key());
            player->sendLine(QString("&W    Resource&d: %1 (%2)")
                             .arg(QMUD::capitalize(strResource).leftJustified(20))
                             .arg(it.value()));
        }
    }
}

void CommandSuInfo::sendItemList(Player *player, QString filter, QMUD::IdType firstId)
{
    int counter = 0;
    auto items = QMUD::qmudWorld->items();

    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if (it.key() >= firstId)
        {
            if (it.value()->name().contains(filter, Qt::CaseInsensitive))
            {
                player->sendLine(QString(" %1 - %2 [MAP:%3] [CHI:%4:%5]")
                                 .arg(it.key(), 6)
                                 .arg(QString(it.value()->name()).leftJustified(40, '.', true))
                                 .arg(it.value()->mapStaticId())
                                 .arg(it.value()->parent() ? static_cast<Entity*>(it.value()->parent())->name(Q_NULLPTR) : "NULL")
                                 .arg(it.value()->parent() ? static_cast<Entity*>(it.value()->parent())->id() : 0));
                counter++;

                if (counter >= 50)
                    break;
            }
        }
    }

    if (counter == 0)
    {
        player->sendLine(QObject::tr("Nessun oggetto trovato"));
    }
}

void CommandSuInfo::sendItemInfo(Item *item, Player *player)
{
    player->sendLine(QString("&W        Name&d: %1").arg(item->name()));
    player->sendLine(QString("&W     Quality&d: %1").arg(QMUD::itemQualityToReadableString(item->quality(), false)));
    player->sendLine(QString("&W Description&d: %1").arg(item->description()));
    player->sendLine(QString("&W    StaticId&d: %1").arg(item->staticId()));
    player->sendLine(QString("&W MapStaticId&d: %1").arg(item->mapStaticId()));
    player->sendLine(QString("&W          Id&d: %1").arg(item->id()));
    player->sendLine(QString("&W      IsBind&d: %1").arg(item->isBind() ? "YES" : "NO"));
    player->sendLine(QString("&W       Level&d: %1").arg(item->level()));
    player->send    (QString("&W        UUID&d: "));    player->sendLineNoColorConversion(item->uuid().toString());

    if (item->parent())
        player->sendLine(QString("&W      Parent&d: %1 (%2)")
                         .arg(static_cast<Entity*>(item->parent())->name(Q_NULLPTR))
                         .arg(static_cast<Entity*>(item->parent())->id())
                         );
    else
        player->sendLine(QString("&W      Parent&d: NULL"));
    player->sendLine(QString("&W       Value&d: %1&d").arg(QMUD::coinsToReadableString(item->coinsValue())));
    player->sendLine(QString("&W        Type&d: %1").arg(QMUD::itemTypeToString(item->itemType())));

    if (item->itemType() == QMUD::ItemType::ARMOR)
    {
        auto armor = static_cast<ItemArmor*>(item);
        player->sendLine(QString("&W   ArmorType&d: %1").arg(QMUD::armorTypeToString(armor->armorType())));
        player->sendLine(QString("&W   ClassType&d: %1").arg(QMUD::itemClassToString(armor->classType())));
        player->sendLine(QString("&W     ACBonus&d: %1").arg(armor->armorClassBonus()));
    }
    else if (item->itemType() == QMUD::ItemType::WEAPON)
    {
        auto weapon = static_cast<ItemWeapon*>(item);
        player->sendLine(QString("&W  WeaponType&d: %1").arg(QMUD::weaponTypeToString(weapon->weaponType())));
        player->sendLine(QString("&W   ClassType&d: %1").arg(QMUD::itemClassToString(weapon->classType())));
        player->sendLine(QString("&W  DamageType&d: %1").arg(QMUD::damageTypeToString(weapon->damageType())));
        player->sendLine(QString("&W       Speed&d: %1").arg(weapon->speed()));
        player->sendLine(QString("&W      Damage&d: %1 - %2").arg(weapon->damageMin()).arg(weapon->damageMax()));
    }
    else if (item->itemType() == QMUD::ItemType::TRINKET)
    {
        auto trinket = static_cast<ItemTrinket*>(item);
        player->sendLine(QString("&W TrinketType&d: %1").arg(QMUD::trinketTypeToString(trinket->trinketType())));
        player->sendLine(QString("&W   ClassType&d: %1").arg(QMUD::itemClassToString(trinket->classType())));
    }
    else if (item->itemType() == QMUD::ItemType::FOOD)
    {
        auto food = static_cast<ItemFood*>(item);
        player->sendLine(QString("&W HpGainBonus&d: %1").arg(food->hpGainBonus()));
        player->sendLine(QString("&W ApGainBonus&d: %1").arg(food->apGainBonus()));
    }
    else if (item->itemType() == QMUD::ItemType::DRINK)
    {
        auto drink = static_cast<ItemDrink*>(item);
        player->sendLine(QString("&W MpGainBonus&d: %1").arg(drink->mpGainBonus()));
    }
    else if (item->itemType() == QMUD::ItemType::POTION)
    {
        auto potion = static_cast<ItemPotion*>(item);
        player->sendLine(QString("&W DurationSec&d: %1").arg(potion->durationSeconds()));
    }
    else if (item->itemType() == QMUD::ItemType::TONIC)
    {
        auto tonic = static_cast<ItemTonic*>(item);
        player->sendLine(QString("&W DurationSec&d: %1").arg(tonic->durationSeconds()));
    }
    else if (item->itemType() == QMUD::ItemType::RESOURCE)
    {
        auto resource = static_cast<ItemResource*>(item);
        player->sendLine(QString("&W    Resource&d: %1").arg(QMUD::resourceToString(resource->resource())));
        player->sendLine(QString("&W Collectable&d: %1").arg(resource->collectable() ? "YES" : "NO"));
        player->sendLine(QString("&W      Action&d: %1").arg(resource->action()));
        player->sendLine(QString("&W ActionOther&d: %1").arg(resource->actionOther()));
    }

    sendItemBaseBonusInfo(item->baseBonus(), player);
    sendItemBaseTemporaryStatusInfo(item->baseTemporaryStatus(), player);
}

void CommandSuInfo::sendItemBaseBonusInfo(const QMUD::ItemBaseBonus &baseBonus, Player* player)
{
    if (baseBonus.constitution() != 0)
        player->sendLine(QString("&W    ConBonus&d: %1").arg(baseBonus.constitution()));
    if (baseBonus.dexterity() != 0)
        player->sendLine(QString("&W    DexBonus&d: %1").arg(baseBonus.dexterity()));
    if (baseBonus.intelligence() != 0)
        player->sendLine(QString("&W    IntBonus&d: %1").arg(baseBonus.intelligence()));
    if (baseBonus.wisdom() != 0)
        player->sendLine(QString("&W    WisBonus&d: %1").arg(baseBonus.wisdom()));
    if (baseBonus.charisma() != 0)
        player->sendLine(QString("&W    ChaBonus&d: %1").arg(baseBonus.charisma()));
    if (baseBonus.strength() != 0)
        player->sendLine(QString("&W   StreBonus&d: %1").arg(baseBonus.strength()));
    if (baseBonus.hit() != 0)
        player->sendLine(QString("&W    HitBonus&d: %1").arg(baseBonus.hit()));
    if (baseBonus.damage() != 0)
        player->sendLine(QString("&W DamageBonus&d: %1").arg(baseBonus.damage()));
    if (baseBonus.hitPoints() != 0)
        player->sendLine(QString("&W     HPBonus&d: %1").arg(baseBonus.hitPoints()));
    if (baseBonus.manaPoints() != 0)
        player->sendLine(QString("&W   ManaBonus&d: %1").arg(baseBonus.manaPoints()));
    if (baseBonus.actionPoints() != 0)
        player->sendLine(QString("&W     APBonus&d: %1").arg(baseBonus.actionPoints()));
}

void CommandSuInfo::sendItemBaseTemporaryStatusInfo(const QMUD::ItemBaseStatus &baseStatus, Player *player)
{
    if (!baseStatus.temporaryStatus().isEmpty())
    {
        QStringList statusList;
        for (auto status : baseStatus.temporaryStatus())
             statusList.push_back(QMUD::statusToString(status));

        player->sendLine(QString("&W      Status&d: %1").arg(statusList.join(",")));
    }
}

void CommandSuInfo::sendMapInfo(Map *map, Player *player)
{
    player->sendLine(QString(" %1 - %2 [id:%3, rooms:%4]")
                     .arg(map->staticId(), 6)
                     .arg(map->name())
                     .arg(map->id())
                     .arg(map->rooms().size()));

    auto indexes = map->wildMapsIndexes();

    for (auto index : indexes)
        player->sendLine(QObject::tr("Mappa %1 - Size: %2x%3").arg(map->wildMapName(index)).arg(map->width(index)).arg(map->height(index)));
}

void CommandSuInfo::sendRoomInfo(Room *room, Player *player)
{
    player->send("&w");
    player->sendLine(QString("&W        Name&d: %1").arg(room->name()));
    player->sendLine(QString("&W Description&d: " QMUD_GLOBAL_NEW_LINE "%1&d").arg(room->description(Q_NULLPTR)));
    player->sendLine(QString("&W    StaticId&d: %1").arg(room->staticId()));
    player->sendLine(QString("&W MapStaticId&d: %1").arg(room->map()->staticId()));
    player->sendLine(QString("&W          Id&d: %1").arg(room->id()));
    player->sendLine(QString("&W       Group&d: %1").arg(room->group()));

    QStringList flagsStr;
    auto flags = room->flags();

    for (auto f : flags)
        flagsStr.push_back(QMUD::roomFlagToString(f));

    player->sendLine(QString("&W       Flags&d: %1").arg(flagsStr.join(" ")));
    player->sendLine(QString("&W       WildX&d: %1").arg(room->wildX()));
    player->sendLine(QString("&W       WildY&d: %1").arg(room->wildY()));
}
