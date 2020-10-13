/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandinfo.h"
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

CommandInfo::CommandInfo() :
    Command()
{

}

CommandInfo::~CommandInfo()
{

}

bool CommandInfo::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("informazioni"), cmd) ||
           Command::isOf(QObject::tr("informazione"), cmd);
}

QStringList CommandInfo::commands()
{
    return QStringList() << QObject::tr("informazioni");
}

void CommandInfo::executePc(Character *ch, Player* player)
{
    if (args().size() != 1)
    {
        sendUsage(player);
        return;
    }

    Item* item = Q_NULLPTR;

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
    }
    else
    {
        bool wrongUsage = false;
        item = findItemInInventory(args()[0], ch->inventory(true), wrongUsage);

        if (wrongUsage)
        {
            if (player)
                sendUsage(player);
            return;
        }
    }

    if (!item)
    {
        if (player)
            player->sendLine(QObject::tr("Non hai nessun '%1'").arg(args()[0]));
        return;
    }

    sendItemInfo(item, player);
}

void CommandInfo::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandInfo::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: info <nome>"));
}

void CommandInfo::sendItemInfo(Item *item, Player *player)
{
    player->sendLine(QString("%1%2&d [%3]").arg(QMUD::itemQualityToColor(item->quality())).arg(item->name()).arg(QMUD::itemTypeToReadableString(item->itemType())));
    player->sendLine(QString("%1").arg(item->description()));
    player->sendLine(QObject::tr("&WLivello&d: %1").arg(item->level()));
    player->sendLine(QObject::tr("&W Valore&d: %1&d").arg(QMUD::coinsToReadableString(item->coinsValue())));

    if (item->itemType() == QMUD::ItemType::ARMOR)
    {
        auto armor = static_cast<ItemArmor*>(item);
        player->sendLine(QObject::tr("&W   Tipo&d: %1").arg(QMUD::armorTypeToReadableString(armor->armorType())));
        player->sendLine(QObject::tr("&W     AC&d: %1").arg(armor->armorClassBonus()));
    }
    else if (item->itemType() == QMUD::ItemType::WEAPON)
    {
        auto weapon = static_cast<ItemWeapon*>(item);
        player->sendLine(QObject::tr("&W   Tipo&d: %1").arg(QMUD::weaponTypeToReadableString(weapon->weaponType())));
        player->sendLine(QObject::tr("&W  Danno&d: %1 - %2 (velocita' %3)").arg(weapon->damageMin()).arg(weapon->damageMax()).arg(weapon->speed()));
    }
    else if (item->itemType() == QMUD::ItemType::TRINKET)
    {
        auto trinket = static_cast<ItemTrinket*>(item);
        player->sendLine(QObject::tr("&W   Tipo&d: %1").arg(QMUD::trinketTypeToReadableString(trinket->trinketType())));
    }
    else if (item->itemType() == QMUD::ItemType::FOOD)
    {
        auto food = static_cast<ItemFood*>(item);
        player->sendLine(QObject::tr("&W  Bonus&d: +%1 HP, +%2 AP").arg(food->hpGainBonus()).arg(food->apGainBonus()));
    }
    else if (item->itemType() == QMUD::ItemType::DRINK)
    {
        auto drink = static_cast<ItemDrink*>(item);
        player->sendLine(QObject::tr("&W  Bonus&d: +%1 MP").arg(drink->mpGainBonus()));
    }
    else if (item->itemType() == QMUD::ItemType::POTION)
    {
        auto potion = static_cast<ItemPotion*>(item);
        player->sendLine(QObject::tr("&W Durata&d: %1 secondi").arg(potion->durationSeconds()));
    }
    else if (item->itemType() == QMUD::ItemType::TONIC)
    {
        auto tonic = static_cast<ItemTonic*>(item);
        player->sendLine(QObject::tr("&W Durata&d: %1 secondi").arg(tonic->durationSeconds()));
    }

    sendItemBaseBonusInfo(item->baseBonus(), player);
    sendItemBaseTemporaryStatusInfo(item->baseTemporaryStatus(), player);

    player->sendLine(QObject::tr("&W Azioni&d: [%[%1]&C&uSACRIFICA&d%[]]").arg(QObject::tr("SACRIFICO") + " " + QString::number(item->id())));
}

void CommandInfo::sendItemBaseBonusInfo(const QMUD::ItemBaseBonus &baseBonus, Player* player)
{
    if (baseBonus.constitution() != 0)
        player->sendLine(QObject::tr("&W    Con&d: +%1").arg(baseBonus.constitution()));
    if (baseBonus.dexterity() != 0)
        player->sendLine(QObject::tr("&W    Dex&d: +%1").arg(baseBonus.dexterity()));
    if (baseBonus.intelligence() != 0)
        player->sendLine(QObject::tr("&W    Int&d: +%1").arg(baseBonus.intelligence()));
    if (baseBonus.wisdom() != 0)
        player->sendLine(QObject::tr("&W    Sag&d: +%1").arg(baseBonus.wisdom()));
    if (baseBonus.charisma() != 0)
        player->sendLine(QObject::tr("&W    Car&d: +%1").arg(baseBonus.charisma()));
    if (baseBonus.strength() != 0)
        player->sendLine(QObject::tr("&W    For&d: +%1").arg(baseBonus.strength()));
    if (baseBonus.hit() != 0)
        player->sendLine(QObject::tr("&WColpire&d: +%1").arg(baseBonus.hit()));
    if (baseBonus.damage() != 0)
        player->sendLine(QObject::tr("&W  Danno&d: +%1").arg(baseBonus.damage()));
    if (baseBonus.hitPoints() != 0)
        player->sendLine(QObject::tr("&W     HP&d: +%1").arg(baseBonus.hitPoints()));
    if (baseBonus.manaPoints() != 0)
        player->sendLine(QObject::tr("&W     MP&d: +%1").arg(baseBonus.manaPoints()));
    if (baseBonus.actionPoints() != 0)
        player->sendLine(QObject::tr("&W     AP&d: +%1").arg(baseBonus.actionPoints()));
}

void CommandInfo::sendItemBaseTemporaryStatusInfo(const QMUD::ItemBaseStatus &baseStatus, Player *player)
{
    if (!baseStatus.temporaryStatus().isEmpty())
    {
        QStringList statusList;
        for (auto status : baseStatus.temporaryStatus())
             statusList.push_back(QMUD::statusToReadableString(status));

        player->sendLine(QObject::tr("&W  Stato&d: %1").arg(statusList.join(", ")));
    }
}
