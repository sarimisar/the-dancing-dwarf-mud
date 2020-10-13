/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandequipment.h"
#include "world.h"
#include "item.h"

using namespace CORE;

CommandEquipment::CommandEquipment() :
    Command()
{

}

CommandEquipment::~CommandEquipment()
{

}

bool CommandEquipment::isOf(QString cmd)
{
    return Command::isOf(QObject::tr("equipaggiamento"), cmd);
}

QStringList CommandEquipment::commands()
{
    return QStringList() << QObject::tr("equipaggiamento");
}

void CommandEquipment::executePc(Character *ch, Player* player)
{
    int equippedItemsCount = 0;

    Item* weapon = ch->equippedItem(Character::EquipmentSlot::EQSLOT_WEAPON);
    Item* chest = ch->equippedItem(Character::EquipmentSlot::EQSLOT_CHEST);
    Item* shield = ch->equippedItem(Character::EquipmentSlot::EQSLOT_SHIELD);
    Item* gloves = ch->equippedItem(Character::EquipmentSlot::EQSLOT_GLOVES);
    Item* shoulder = ch->equippedItem(Character::EquipmentSlot::EQSLOT_SHOULDER);
    Item* bracers = ch->equippedItem(Character::EquipmentSlot::EQSLOT_BRACERS);
    Item* helm = ch->equippedItem(Character::EquipmentSlot::EQSLOT_HELM);
    Item* leg = ch->equippedItem(Character::EquipmentSlot::EQSLOT_LEG);
    Item* boots = ch->equippedItem(Character::EquipmentSlot::EQSLOT_BOOTS);
    Item* ringLeft = ch->equippedItem(Character::EquipmentSlot::EQSLOT_RING_LEFT);
    Item* ringRight = ch->equippedItem(Character::EquipmentSlot::EQSLOT_RING_RIGHT);
    Item* braceletLeft = ch->equippedItem(Character::EquipmentSlot::EQSLOT_BRACELET_LEFT);
    Item* braceletRight = ch->equippedItem(Character::EquipmentSlot::EQSLOT_BRACELET_RIGHT);
    Item* necklace = ch->equippedItem(Character::EquipmentSlot::EQSLOT_NECKLACE);

    if (weapon)
    {
        player->sendLine(QObject::tr("  Impugnato     - %2%1&d").arg(QMUD::capitalize(weapon->name())).arg(QMUD::itemQualityToColor(weapon->quality())));
        equippedItemsCount++;
    }

    if (helm)
    {
        player->sendLine(QObject::tr("  Sulla testa   - %2%1&d").arg(QMUD::capitalize(helm->name())).arg(QMUD::itemQualityToColor(helm->quality())));
        equippedItemsCount++;
    }

    if (chest)
    {
        player->sendLine(QObject::tr("  Sul corpo     - %2%1&d").arg(QMUD::capitalize(chest->name())).arg(QMUD::itemQualityToColor(chest->quality())));
        equippedItemsCount++;
    }

    if (shield)
    {
        player->sendLine(QObject::tr("  Scudo         - %2%1&d").arg(QMUD::capitalize(shield->name())).arg(QMUD::itemQualityToColor(shield->quality())));
        equippedItemsCount++;
    }

    if (gloves)
    {
        player->sendLine(QObject::tr("  Sulle mani    - %2%1&d").arg(QMUD::capitalize(gloves->name())).arg(QMUD::itemQualityToColor(gloves->quality())));
        equippedItemsCount++;
    }

    if (shoulder)
    {
        player->sendLine(QObject::tr("  Sulle spalle  - %2%1&d").arg(QMUD::capitalize(shoulder->name())).arg(QMUD::itemQualityToColor(shoulder->quality())));
        equippedItemsCount++;
    }

    if (bracers)
    {
        player->sendLine(QObject::tr("  Sulle braccia - %2%1&d").arg(QMUD::capitalize(bracers->name())).arg(QMUD::itemQualityToColor(bracers->quality())));
        equippedItemsCount++;
    }

    if (leg)
    {
        player->sendLine(QObject::tr("  Sulle gambe   - %2%1&d").arg(QMUD::capitalize(leg->name())).arg(QMUD::itemQualityToColor(leg->quality())));
        equippedItemsCount++;
    }

    if (boots)
    {
        player->sendLine(QObject::tr("  Sui piedi     - %2%1&d").arg(QMUD::capitalize(boots->name())).arg(QMUD::itemQualityToColor(boots->quality())));
        equippedItemsCount++;
    }

    if (ringLeft)
    {
        player->sendLine(QObject::tr("  Anello        - %2%1&d").arg(QMUD::capitalize(ringLeft->name())).arg(QMUD::itemQualityToColor(ringLeft->quality())));
        equippedItemsCount++;
    }

    if (ringRight)
    {
        player->sendLine(QObject::tr("  Anello        - %2%1&d").arg(QMUD::capitalize(ringRight->name())).arg(QMUD::itemQualityToColor(ringRight->quality())));
        equippedItemsCount++;
    }

    if (braceletLeft)
    {
        player->sendLine(QObject::tr("  Braccialetto  - %2%1&d").arg(QMUD::capitalize(braceletLeft->name())).arg(QMUD::itemQualityToColor(braceletLeft->quality())));
        equippedItemsCount++;
    }

    if (braceletRight)
    {
        player->sendLine(QObject::tr("  Braccialetto  - %2%1&d").arg(QMUD::capitalize(braceletRight->name())).arg(QMUD::itemQualityToColor(braceletRight->quality())));
        equippedItemsCount++;
    }

    if (necklace)
    {
        player->sendLine(QObject::tr("  Al collo      - %2%1&d").arg(QMUD::capitalize(necklace->name())).arg(QMUD::itemQualityToColor(necklace->quality())));
        equippedItemsCount++;
    }

    player->sendLine(QObject::tr("&WHai equipaggiato %1 oggetti&d").
                     arg(equippedItemsCount));
}

void CommandEquipment::executeNpc(Character* ch)
{
    Q_UNUSED(ch)
}

void CommandEquipment::sendUsage(Player *player, bool help)
{
    if (help)
    {
        player->sendLine(QObject::tr("Mostra l'elenco degli oggetto equipaggiati"));
    }

    player->sendLine(QObject::tr("Uso: equipaggiamento"));
}
