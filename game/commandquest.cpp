/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "commandquest.h"
#include "world.h"
#include "room.h"
#include "item.h"
#include "npccharacter.h"
#include "globaldebug.h"

using namespace CORE;

CommandQuest::CommandQuest(ActionType action) :
    Command(),
    m_eActionType(action)
{

}

CommandQuest::~CommandQuest()
{

}

bool CommandQuest::isOf(QString cmd, ActionType &action)
{
    if (Command::isOf(QObject::tr("quest"), cmd))
    {
        action = ActionType::QUEST;
        return true;
    }
    else if (Command::isOf(QObject::tr("diario"), cmd))
    {
        action = ActionType::DIARY;
        return true;
    }

    return false;
}

QStringList CommandQuest::commands()
{
    return QStringList() << QObject::tr("quest")
                         << QObject::tr("diario");
}

void CommandQuest::executePc(Character *ch, Player* player)
{
    if (ch->isUnconscious())
    {
        if (player)
            player->sendLine(QObject::tr("Sei incoscente."));

        return;
    }

    if (m_eActionType == ActionType::QUEST)
    {
        if (args().isEmpty())
        {
            bool isEmpty = true;
            auto characters = ch->room()->characters();

            QVector<Quest::QuestIdType> alreadyAdded;
            PcCharacter *pc = static_cast<PcCharacter*>(ch);

            for (auto it = characters.begin(); it != characters.end(); ++it)
            {
                if ((*it)->chType() == Character::ChType::CHTYPE_NPC)
                {
                    NpcCharacter *npc = static_cast<NpcCharacter*>(*it);

                    if (ch->canSee(npc))
                    {
                        auto quests = npc->questsBeGived();

                        if (!quests.isEmpty())
                        {
                            for (auto quest : quests)
                            {
                                if (!alreadyAdded.contains(quest->id()))
                                {
                                    if (pc->questIsSuitabled(quest))
                                    {
                                        if (isEmpty)
                                            player->sendLine(QObject::tr("%1:").arg(npc->name(Q_NULLPTR)));

                                        isEmpty = false;

                                        player->sendLine(QObject::tr("[%[%1]&C&uINFO&d%[]] [%[%2]&C&uACCETTA&d%[]]%3 %4 - %5 [LVL %6]&d")
                                                         .arg(QObject::tr("quest informazioni ") + quest->idToString())
                                                         .arg(QObject::tr("quest accetto ") + quest->idToString())
                                                         .arg(quest->level() <= ch->level() ? "&Y" : "&r")
                                                         .arg(quest->idToString())
                                                         .arg(quest->name())
                                                         .arg(quest->level()));
                                        alreadyAdded.push_back(quest->id());
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (isEmpty)
                player->sendLine(QObject::tr("Non c'e' nessuna quest disponibile qui"));
        }
        else if (args().size() == 2 && (QObject::tr("informazioni").startsWith(args()[0]) || QObject::tr("informazione").startsWith(args()[0])))
        {
            QString questId = args()[1];
            auto characters = ch->room()->characters();

            PcCharacter *pc = static_cast<PcCharacter*>(ch);

            for (auto it = characters.begin(); it != characters.end(); ++it)
            {
                if ((*it)->chType() == Character::ChType::CHTYPE_NPC)
                {
                    NpcCharacter *npc = static_cast<NpcCharacter*>(*it);

                    if (ch->canSee(npc))
                    {
                        auto quests = npc->questsBeGived();

                        if (!quests.isEmpty())
                        {
                            for (auto quest : quests)
                            {
                                if (quest->idToString() == questId)
                                {
                                    player->sendLine("&Y--------------------------------------------------------&d");
                                    player->sendLine(QObject::tr("&Y%1 %2&d").arg(quest->idToString()).arg(quest->name()));
                                    player->sendLine(quest->giveDescription().arg(ch->name(Q_NULLPTR)));

                                    if (!quest->targetData().isEmpty())
                                    {
                                        player->sendLine("");
                                        player->sendLine("&YOBBIETTIVI:&d");

                                        auto activeQuests = pc->questListActive();
                                        auto it = activeQuests.find(quest->id());

                                        if (it != activeQuests.end())
                                        {
                                            it.value().sendProgress(player);
                                        }
                                        else
                                        {
                                            for (const auto& target : quest->targetData())
                                                player->sendLine(target.description());
                                        }

                                        player->sendLine("");
                                    }

                                    player->sendLine(QObject::tr("[%[%1]&C&uACCETTA&d%[]]&d")
                                                     .arg(QObject::tr("quest accetto ") + quest->idToString()));
                                    player->sendLine("&Y--------------------------------------------------------&d");

                                    PcCharacter *pc = static_cast<PcCharacter*>(ch);

                                    pc->questAccept(quest);
                                    return;
                                }
                            }
                        }
                    }
                }
            }

            player->sendLine(QObject::tr("Non ci sono informazioni su questa quest"));
            return;
        }
        else if (args().size() == 2 && (QObject::tr("accetto").startsWith(args()[0]) || QObject::tr("accetta").startsWith(args()[0])))
        {
            QString questId = args()[1];
            auto characters = ch->room()->characters();

            for (auto it = characters.begin(); it != characters.end(); ++it)
            {
                if ((*it)->chType() == Character::ChType::CHTYPE_NPC)
                {
                    NpcCharacter *npc = static_cast<NpcCharacter*>(*it);

                    if (ch->canSee(npc))
                    {
                        auto quests = npc->questsBeGived();

                        if (!quests.isEmpty())
                        {
                            for (auto quest : quests)
                            {
                                if (quest->idToString() == questId)
                                {
                                    if (ch->level() < quest->level())
                                    {
                                        player->sendLine(QObject::tr("Non hai il livello adatto per la quest &Y[%1] %2&d").arg(quest->idToString()).arg(quest->name()));
                                        return;
                                    }

                                    player->sendLine(QObject::tr("Accetti la quest &Y[%1] %2&d").arg(quest->idToString()).arg(quest->name()));

                                    PcCharacter *pc = static_cast<PcCharacter*>(ch);

                                    pc->questAccept(quest);
                                    return;
                                }
                            }
                        }
                    }
                }
            }

            player->sendLine(QObject::tr("Non puoi accettare questa quest qui"));
            return;
        }
        else if (args().size() == 3 && (QObject::tr("seguo").startsWith(args()[0]) || QObject::tr("segui").startsWith(args()[0])))
        {
            Quest::QuestIdType questId = Quest::idFromString(args()[1]);

            if (questId == Quest::QuestIdInvalid)
            {
                player->sendLine(QObject::tr("Non hai nessuna quest con id %1 attiva").arg(args()[1]));
                return;
            }

            PcCharacter *pc = static_cast<PcCharacter*>(ch);

            auto activeQuests = pc->questListActive();
            auto itQuest = activeQuests.find(questId);

            if (itQuest == activeQuests.end())
            {
                player->sendLine(QObject::tr("Non hai nessuna quest con id %1 attiva").arg(args()[1]));
                return;
            }

            auto quest = itQuest.value().quest();
            int targetIndex = args()[2].toInt();

            if (targetIndex < 0 || targetIndex >= quest->targetData().size())
            {
                player->sendLine(QObject::tr("Obbiettivo %1 non valido. Usa un numero da 0 a %2").arg(targetIndex).arg(quest->targetData().size() - 1));
                return;
            }

            QMUD::StaticIdType mapId;
            QMUD::StaticIdType npcId;

            if (quest->targetData()[targetIndex].npcToTalkToEnabled())
            {
                mapId = quest->targetData()[targetIndex].npcToTalkToMapId();
                npcId = quest->targetData()[targetIndex].npcToTalkToId();
            }
            else if (quest->targetData()[targetIndex].npcToKillEnabled())
            {
                mapId = quest->targetData()[targetIndex].npcToKillMapId();
                npcId = quest->targetData()[targetIndex].npcToKillId();
            }
            else
            {
                player->sendLine(QObject::tr("Non puoi seguire l'obbiettivo &Y%1&d").arg(quest->targetData()[targetIndex].description()));
                return;
            }

            auto map = QMUD::qmudWorld->map(mapId);

            if (!map)
            {
                player->sendLine(QObject::tr("Non puoi seguire l'obbiettivo &Y%1&d").arg(quest->targetData()[targetIndex].description()));
                return;
            }

            auto rooms = map->npcRoomByStaticId(npcId);

            if (rooms.isEmpty())
            {
                player->sendLine(QObject::tr("Non puoi seguire l'obbiettivo &Y%1&d").arg(quest->targetData()[targetIndex].description()));
                return;
            }

            player->sendLine(QObject::tr("Inizi a seguire &Y%1&d").arg(quest->targetData()[targetIndex].description()));
            pc->setTracedRoom(rooms[0], mapId, npcId);
            pc->sendLook();

            return;
        }
        else if (args().size() == 2 && (QObject::tr("completo").startsWith(args()[0]) || QObject::tr("completa").startsWith(args()[0])))
        {
            PcCharacter *pc = static_cast<PcCharacter*>(ch);

            auto quests = pc->questListActive();

            for (auto quest : pc->questListActive())
            {
                if (quest.quest()->idToString() == args()[1])
                {
                    if (!quest.isCompleted())
                    {
                        player->sendLine(QObject::tr("Non hai ancora completato la quest &Y[%1] %2&d").arg(quest.quest()->idToString()).arg(quest.quest()->name()));
                        return;
                    }

                    auto characters = ch->room()->characters();

                    for (auto chInRoom : characters)
                    {
                        if (chInRoom->chType() == Character::ChType::CHTYPE_NPC)
                        {
                            NpcCharacter *npc = static_cast<NpcCharacter*>(chInRoom);

                            if (quest.quest()->completeNpcId() == npc->staticId() &&
                                quest.quest()->completeNpcMapId() == npc->mapStaticId())
                            {
                                pc->questComplete(quest.quest()->id());
                                player->sendLine("&Y--------------------------------------------------------&d");
                                player->sendLine(QObject::tr("Completi la quest &Y[%1] %2&d").arg(quest.quest()->idToString()).arg(quest.quest()->name()));
                                player->sendLine(quest.quest()->completeDescription().arg(ch->name(Q_NULLPTR)));

                                qint64 experience = NpcCharacter::baseExperience(quest.quest()->level(), quest.quest()->difficulty()) * 5;
                                qint64 copp = NpcCharacter::baseCoins(quest.quest()->level(), quest.quest()->difficulty()) * 8 + quest.quest()->gold();

                                player->sendLine("");
                                player->sendLine(QObject::tr("Guadagni &W%1&d punti esperienza").arg(experience));
                                player->sendLine(QObject::tr("Guadagni &W%1&d").arg(QMUD::coinsToReadableString(copp)));
                                pc->addCoins(copp);

                                if (quest.quest()->giftItemEnabled())
                                {
                                    if (!quest.quest()->specificGiftItemEnabled())
                                    {
                                        QString randomCode = Item::generateRandomCodeForCharacter(ch, quest.quest()->level());
                                        int level;
                                        QMUD::ItemType itemType;
                                        QString name;
                                        QMUD::ItemClassType classType;
                                        QMUD::ItemQualityType quality;

                                        if (Item::fromRandomCodeToValues(randomCode, level, itemType, name, classType, quality))
                                        {
                                            PcCharacter::LootItemData itemLoot(QJsonObject(),
                                                                  randomCode,
                                                                  QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RANDOM_ITEM,
                                                                  quality,
                                                                  QMUD::ItemRarityType::RARITY0,
                                                                  QMUD_GLOBAL_STATIC_ID_ITEM_RANDOM_ITEM,
                                                                  name,
                                                                  false,
                                                                  Item::getDefaultCoinsValue(level, quality));

                                            player->sendLine(QObject::tr("Ottieni %2%1&d").arg(itemLoot.name()).arg(QMUD::itemQualityToColor(itemLoot.quality())));
                                            pc->addLoot(itemLoot);
                                        }
                                    }
                                    else
                                    {
                                        auto map = QMUD::qmudWorld->map(quest.quest()->specificGiftItemMapId());

                                        if (map)
                                        {
                                            auto itemList = map->mapJsonObjItemsById();

                                            auto itItemJson = itemList.find(quest.quest()->specificGiftItemId());

                                            if (itItemJson != itemList.end())
                                            {
                                                PcCharacter::LootItemData itemLoot(QJsonObject(),
                                                                      "",
                                                                      quest.quest()->specificGiftItemMapId(),
                                                                      Item::jsonToQuality(itItemJson.value()),
                                                                      QMUD::ItemRarityType::RARITY0,
                                                                      quest.quest()->specificGiftItemId(),
                                                                      Item::jsonToName(itItemJson.value()),
                                                                      false,
                                                                      Item::jsonToSellValue(itItemJson.value()));

                                                player->sendLine(QObject::tr("Ottieni %2%1&d").arg(itemLoot.name()).arg(QMUD::itemQualityToColor(itemLoot.quality())));
                                                pc->addLoot(itemLoot);
                                            }
                                        }
                                    }
                                }

                                player->sendLine("&Y--------------------------------------------------------&d");

                                pc->addExperience(experience);

                                return;
                            }
                        }
                    }
                }
            }

            player->sendLine(QObject::tr("Non hai questa quest nel tuo diario"));
            return;
        }
        else
        {
            sendUsage(player);
        }
    }
    else if (m_eActionType == ActionType::DIARY)
    {
        PcCharacter *pc = static_cast<PcCharacter*>(ch);

        auto quests = pc->questListActive();

        player->sendLine("&Y------------------------- DIARIO -----------------------&d");

        if (quests.isEmpty())
        {
            player->sendLine(QObject::tr("Non hai nessuna quest nel tuo diario"));
        }
        else
        {
            for (auto quest : quests)
            {
                player->sendLine(QObject::tr("[%[%1]&C&uINFO&d%[]] &Y%2 - %3 [LVL %4]%5&d")
                                 .arg(QObject::tr("quest informazioni ") + quest.quest()->idToString())
                                 .arg(quest.quest()->idToString())
                                 .arg(quest.quest()->name())
                                 .arg(quest.quest()->level())
                                 .arg(quest.isCompleted() ? QObject::tr(" [%[quest completo %1]&C&uCOMPLETA&d%[]]").arg(quest.quest()->idToString()) : ""));
                quest.sendProgress(player);
            }
        }

        if (args().size() == 1 && QObject::tr("completate").startsWith(args()[0]))
        {
            auto questsCompleted = pc->questListCompleted();

            player->sendLine("&Y----------------------- COMPLETATE ---------------------&d");

            if (questsCompleted.isEmpty())
            {
                player->sendLine(QObject::tr("Non hai completato nessuna quest"));
            }
            else
            {
                for (auto quest : questsCompleted)
                {
                    player->sendLine(QObject::tr("%1 - %2 [LVL %3]&d")
                                     .arg(quest->idToString())
                                     .arg(quest->name())
                                     .arg(quest->level()));
                }
            }
        }

        player->sendLine("&Y--------------------------------------------------------&d");
    }
}

void CommandQuest::executeNpc(Character* ch)
{
    Q_UNUSED(ch)

    Q_ASSERT(false);
}

void CommandQuest::sendUsage(Player *player, bool help)
{
    Q_UNUSED(help)

    player->sendLine(QObject::tr("Uso: quest               - Mostra la lista delle quest" QMUD_GLOBAL_NEW_LINE
                                 "     quest accetto <id>  - Accettare una quest tra quelle disponibili" QMUD_GLOBAL_NEW_LINE
                                 "     quest completo <id> - Completare una quest" QMUD_GLOBAL_NEW_LINE
                                 "     diario [completate] - Mostra il diario delle quest attiva [completate]"));
}

