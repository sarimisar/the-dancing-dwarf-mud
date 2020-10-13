/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandlook.h"
#include "world.h"
#include "npccharacter.h"
#include "room.h"
#include "item.h"
#include "color.h"
#include "../common.h"

using namespace CORE;

CommandLook::CommandLook(ActionType type) :
    Command(),
    m_eActionType(type)
{

}

CommandLook::~CommandLook()
{

}

bool CommandLook::isOf(QString cmd, ActionType &type)
{
    if (Command::isOf(QObject::tr("guarda"), cmd) ||
           Command::isOf(QObject::tr("guardo"), cmd))
    {
        type = ActionType::LOOK;
        return true;
    }
    else if (Command::isOf(QObject::tr("guardanp"), cmd))
    {
        type = ActionType::LOOK_NO_PROMPT;
        return true;
    }

    return false;
}

QStringList CommandLook::commands()
{
    return QStringList() << QObject::tr("guardo");
}

void CommandLook::executePc(Character* ch, Player* player)
{
    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    if (args().isEmpty() && ch->chType() == Character::ChType::CHTYPE_PC)
    {
        Q_ASSERT(!!player);

        Room* room = ch->room();

        // Nome della stanza
        QString roomName = room->name();

        // Descrizione
        QString roomDescription = room->description(ch);






//        // Portali
//        QStringList roomContent;
//        if (!room->instanceEntryPoint().isEmpty())
//            roomContent.push_back("&GUn portale che conduce ad una zona nascosta e' qui&d");

//        if (room->instanceExitRoom())
//            roomContent.push_back("&GUn portale che conduce fuori da questa zona e' qui&d");

//        // Characters
//        QVector<Character*> characters;
//        QVector<int> charactersCounter;

//        for (auto it = room->children().begin(); it != room->children().end(); ++it)
//        {
//            if (static_cast<Entity*>(*it)->entityType() == Entity::EntityType::CHARACTER &&
//                    static_cast<Character*>(*it)->chType() == Character::ChType::CHTYPE_NPC)
//            {
//                NpcCharacter* npc = static_cast<NpcCharacter*>(*it);

//                if (!ch->canSee(npc))
//                    continue;

//                bool found = false;

//                for (int i=0; i<characters.size(); ++i)
//                {
//                    if (characters[i]->chType() == Character::ChType::CHTYPE_NPC)
//                    {
//                        NpcCharacter* npcCurrent = static_cast<NpcCharacter*>(characters[i]);

//                        if (npcCurrent->staticId() == npc->staticId() &&
//                            npcCurrent->mapStaticId() == npc->mapStaticId() &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::DIVINE_SHIELD) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::DIVINE_SHIELD) &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::ICE_SHIELD) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::ICE_SHIELD) &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::FIRE_SHIELD) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::FIRE_SHIELD) &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::BLADES_SHIELD) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::BLADES_SHIELD) &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1) &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2) &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3) &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4) &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5) &&
//                            npcCurrent->isDead() == npc->isDead() &&
//                            npcCurrent->isUnconscious() == npc->isUnconscious() &&
//                            npcCurrent->combatTargetId() == npc->combatTargetId() &&
//                            npcCurrent->hasTemporaryStatus(QMUD::TemporaryStatusType::FLY) == npc->hasTemporaryStatus(QMUD::TemporaryStatusType::FLY) &&
//                            npcCurrent->isDead() == npc->isDead())
//                        {
//                            charactersCounter[i]++;
//                            found = true;
//                        }
//                    }
//                }

//                if (!found)
//                {
//                    characters.push_back(npc);
//                    charactersCounter.push_back(1);
//                }
//            }

//            if (static_cast<Entity*>(*it)->entityType() == Entity::EntityType::CHARACTER &&
//                    static_cast<Character*>(*it)->chType() == Character::ChType::CHTYPE_PC)
//            {
//                PcCharacter* pc = static_cast<PcCharacter*>(*it);

//                if (ch == pc || !ch->canSee(pc))
//                    continue;

//                characters.push_back(pc);
//                charactersCounter.push_back(1);
//            }
//        }

//        for (int i = 0; i < characters.size(); ++i)
//        {
//            QString colorLevel;
//            QString strLevel;

//            strLevel = QString::number(characters[i]->level()).leftJustified(3);

//            int levelDiff = characters[i]->level() - ch->level();

//            if (levelDiff < -2)
//                colorLevel = "&z";
//            else if (levelDiff <= 1)
//                colorLevel = "&G";
//            else if (levelDiff <= 4)
//                colorLevel = "&Y";
//            else
//                colorLevel = "&R";
            
//            bool hasQuest = false;

//            if (characters[i]->chType() == Character::ChType::CHTYPE_NPC)
//            {
//                NpcCharacter* npc = static_cast<NpcCharacter*>(characters[i]);

//                auto quests = npc->questsBeGived();

//                if (!quests.isEmpty())
//                {
//                    PcCharacter* pc = static_cast<PcCharacter*>(ch);
//                    for (auto& quest : quests)
//                        if (pc->questIsSuitabled(quest))
//                        {
//                            hasQuest = true;
//                            break;
//                        }
//                }
//            }
            
//            QString commonPart = QString("%1[%2%3&d] %4")
//                    .arg(hasQuest ? "[&Y!&d]" : "")
//                    .arg(colorLevel)
//                    .arg(strLevel)
//                    .arg(QMUD::capitalize(characters[i]->name(Q_NULLPTR)))
//                    + QString(" %1 ")
//                    + (charactersCounter[i] > 1 ? " [" + QString::number(charactersCounter[i]) + "]" : "");

//            if (characters[i]->isDead())
//                roomContent.push_back(commonPart.arg(QObject::tr("e' a terra morto")));
//            else if (characters[i]->isUnconscious())
//                roomContent.push_back(commonPart.arg(QObject::tr("e' a terra privo di sensi")));
//            else if (characters[i]->combatTargetId() == ch->id())
//                roomContent.push_back(commonPart.arg(QObject::tr("sta combattendo con te")));
//            else if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::FLY))
//                roomContent.push_back(commonPart.arg(QObject::tr("sta fluttuando qui")));
//            else
//                roomContent.push_back(commonPart.arg(QObject::tr("e' qui in piedi")));
            
//            if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::DIVINE_SHIELD))
//                roomContent.push_back(QObject::tr("&W > E' avvolto da uno scudo magico&d"));

//            if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::FIRE_SHIELD))
//                roomContent.push_back(QObject::tr("&R > E' avvolto da uno scudo di fuoco&d"));

//            if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::ICE_SHIELD))
//                roomContent.push_back(QObject::tr("&C > E' avvolto da uno scudo di ghiaccio&d"));

//            if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::BLADES_SHIELD))
//                roomContent.push_back(QObject::tr("&z > E' avvolto da uno scudo di lame&d"));

//            if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_1))
//                roomContent.push_back(QObject::tr("&g > E' circondato dalle sue immagini illusorie [1]&d"));
//            else if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_2))
//                roomContent.push_back(QObject::tr("&g > E' circondato dalle sue immagini illusorie [2]&d"));
//            else if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_3))
//                roomContent.push_back(QObject::tr("&g > E' circondato dalle sue immagini illusorie [3]&d"));
//            else if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_4))
//                roomContent.push_back(QObject::tr("&g > E' circondato dalle sue immagini illusorie [4]&d"));
//            else if (characters[i]->hasTemporaryStatus(QMUD::TemporaryStatusType::ILLUSORY_IMAGES_5))
//                roomContent.push_back(QObject::tr("&g > E' circondato dalle sue immagini illusorie [5]&d"));
//        }

//        // Oggetti
//        QVector<Item*> items;
//        QVector<int> itemsCounter;

//        for (auto it = room->children().begin(); it != room->children().end(); ++it)
//        {
//            if (static_cast<Entity*>(*it)->entityType() == Entity::EntityType::ITEM)
//            {
//                Item* item = static_cast<Item*>(*it);

//                bool found = false;

//                for (int i=0; i<items.size(); ++i)
//                {
//                    if (items[i]->staticId() == item->staticId() &&
//                        items[i]->mapStaticId() == item->mapStaticId() &&
//                        items[i]->IsModify() == item->IsModify() &&
//                        items[i]->name() == item->name() &&
//                        items[i]->level() == item->level())
//                    {
//                        itemsCounter[i]++;
//                        found = true;
//                    }
//                }

//                if (!found)
//                {
//                    items.push_back(item);
//                    itemsCounter.push_back(1);
//                }
//            }
//        }

//        for (int i = 0; i < items.size(); ++i)
//        {
//            roomContent.push_back(QObject::tr("%1 e' posato qui%2")
//                             .arg(QMUD::capitalize(items[i]->name()))
//                             .arg(itemsCounter[i] > 1 ? " [" + QString::number(itemsCounter[i]) + "]" : ""));
//        }
    }
    else if (args().size() == 1)
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
            if (args()[0].toLower() == QObject::tr("me"))
                targetEntity = ch;
            else
            {
                if (player)
                    player->sendLine(QObject::tr("'%1' non e' qui").arg(args()[0]));
                return;
            }
        }

        Room* room = ch->room();

        Character *targetCh = qobject_cast<Character*>(targetEntity);
        auto characters = room->characters();

        QString msgCh;
        QString msgTarget;
        QString msgOther;

        if (ch == targetEntity)
        {
            msgCh = QObject::tr("Ti guardi");
            msgOther = QObject::tr("%1 si da un'occhiata").arg(ch->name(Q_NULLPTR));
        }
        else
        {
            if (targetCh)
            {
                msgCh = QObject::tr("Guardi %1").arg(targetCh->name(Q_NULLPTR));

                if (targetCh->chType() == Character::ChType::CHTYPE_NPC)
                    msgCh += QMUD_GLOBAL_NEW_LINE + QObject::tr("%1").arg(qobject_cast<NpcCharacter*>(targetCh)->description());

                msgTarget = QObject::tr("%1 ti guarda").arg(ch->name(Q_NULLPTR));
                msgOther = QObject::tr("%1 guarda %2").arg(ch->name(Q_NULLPTR)).arg(targetCh->name(Q_NULLPTR));
            }
            else
            {
                msgCh = QObject::tr("Guardi %1").arg(targetEntity->name(Q_NULLPTR));
                msgTarget = "";
                msgOther = QObject::tr("%1 guarda %2").arg(ch->name(Q_NULLPTR)).arg(targetEntity->name(Q_NULLPTR));
            }
        }

        for (Character* chInRoom : characters)
        {
            if (chInRoom == ch)
            {
                if (player)
                {
                    player->sendLine(msgCh);

                    if (targetCh)
                    {
                        Item* weapon = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_WEAPON);
                        Item* chest = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_CHEST);
                        Item* shield = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_SHIELD);
                        Item* gloves = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_GLOVES);
                        Item* shoulder = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_SHOULDER);
                        Item* bracers = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_BRACERS);
                        Item* helm = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_HELM);
                        Item* leg = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_LEG);
                        Item* boots = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_BOOTS);
                        Item* ringLeft = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_RING_LEFT);
                        Item* ringRight = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_RING_RIGHT);
                        Item* braceletLeft = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_BRACELET_LEFT);
                        Item* braceletRight = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_BRACELET_RIGHT);
                        Item* necklace = targetCh->equippedItem(Character::EquipmentSlot::EQSLOT_NECKLACE);

                        if (weapon)
                            player->sendLine(QObject::tr("  Impugnato     - %2%1&d").arg(weapon->name()).arg(QMUD::itemQualityToColor(weapon->quality())));

                        if (helm)
                            player->sendLine(QObject::tr("  Sulla testa   - %2%1&d").arg(helm->name()).arg(QMUD::itemQualityToColor(helm->quality())));

                        if (chest)
                            player->sendLine(QObject::tr("  Sul corpo     - %2%1&d").arg(chest->name()).arg(QMUD::itemQualityToColor(chest->quality())));

                        if (shield)
                            player->sendLine(QObject::tr("  Scudo         - %2%1&d").arg(shield->name()).arg(QMUD::itemQualityToColor(shield->quality())));

                        if (gloves)
                            player->sendLine(QObject::tr("  Sulle mani    - %2%1&d").arg(gloves->name()).arg(QMUD::itemQualityToColor(gloves->quality())));

                        if (shoulder)
                            player->sendLine(QObject::tr("  Sulle spalle  - %2%1&d").arg(shoulder->name()).arg(QMUD::itemQualityToColor(shoulder->quality())));

                        if (bracers)
                            player->sendLine(QObject::tr("  Sulle braccia - %2%1&d").arg(bracers->name()).arg(QMUD::itemQualityToColor(bracers->quality())));

                        if (leg)
                            player->sendLine(QObject::tr("  Sulle gambe   - %2%1&d").arg(leg->name()).arg(QMUD::itemQualityToColor(leg->quality())));

                        if (boots)
                            player->sendLine(QObject::tr("  Sui piedi     - %2%1&d").arg(boots->name()).arg(QMUD::itemQualityToColor(boots->quality())));

                        if (ringLeft)
                            player->sendLine(QObject::tr("  Anello        - %2%1&d").arg(ringLeft->name()).arg(QMUD::itemQualityToColor(ringLeft->quality())));

                        if (ringRight)
                            player->sendLine(QObject::tr("  Anello        - %2%1&d").arg(ringRight->name()).arg(QMUD::itemQualityToColor(ringRight->quality())));

                        if (braceletLeft)
                            player->sendLine(QObject::tr("  Braccialetto  - %2%1&d").arg(braceletLeft->name()).arg(QMUD::itemQualityToColor(braceletLeft->quality())));

                        if (braceletRight)
                            player->sendLine(QObject::tr("  Braccialetto  - %2%1&d").arg(braceletRight->name()).arg(QMUD::itemQualityToColor(braceletRight->quality())));

                        if (necklace)
                            player->sendLine(QObject::tr("  Al collo      - %2%1&d").arg(necklace->name()).arg(QMUD::itemQualityToColor(necklace->quality())));
                    }
                }
            }
            else if (chInRoom->chType() == Character::ChType::CHTYPE_PC)
            {
                Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();

                if (chInRoom->canSee(ch) && (!targetCh || chInRoom->canSee(targetCh)))
                {
                    if (targetCh && targetCh == chInRoom)
                        playerOther->sendLine(msgTarget);
                    else
                        playerOther->sendLine(msgOther);
                }
            }
        }
    }
    else
    {
        if (player)
            sendUsage(player);
    }
}

void CommandLook::executeNpc(Character* ch)
{
    executePc(ch, Q_NULLPTR);
}

void CommandLook::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: guarda [<n>.]<nome>"));
}
