/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "npccharacter.h"
#include "room.h"
#include "../global.h"
#include "../globaldebug.h"
#include "characterclass.h"
#include "item.h"
#include "world.h"
#include "command.h"

#include <QJsonArray>

#include <chrono>

using namespace CORE;

// ---------------------------------------------
// Statistiche
// ---------------------------------------------
#define QMUD_NPC_DATA_BY_TYPE_AC_MULT         0

static const double NpcDataByType[static_cast<int>(QMUD::NpcType::MAX_VALUE)][static_cast<int>(QMUD::ClassType::MAX_VALUE)][1] = {
    {
        { 0.6 },
        { 0.3 },
        { 0.5 },
        { 0.3 },
        { 0.3 },
        { 0.6 },
        { 0.5 }
    },
    {
        { 0.8 },
        { 0.4 },
        { 0.6 },
        { 0.4 },
        { 0.4 },
        { 0.8 },
        { 0.6 }
    },
    {
        { 1.0 },
        { 0.6 },
        { 0.8 },
        { 0.6 },
        { 0.6 },
        { 1.0 },
        { 0.8 }
    },
    {
        { 1.0 },
        { 0.6 },
        { 0.8 },
        { 0.6 },
        { 0.6 },
        { 1.0 },
        { 0.8 }
    },
    {
        { 1.0 },
        { 0.6 },
        { 0.8 },
        { 0.6 },
        { 0.6 },
        { 1.0 },
        { 0.8 }
    },
    {
        { 1.0 },
        { 0.6 },
        { 0.8 },
        { 0.6 },
        { 0.6 },
        { 1.0 },
        { 0.8 }
    }
};

NpcCharacter::~NpcCharacter()
{
    GLOBAL_DEBUG_DESTRUCTOR("NpcCharacter", name(Q_NULLPTR));
}

NpcCharacter *NpcCharacter::loadNpcCharacter(QJsonObject &obj, QMap<QMUD::StaticIdType, QJsonObject> objs, QMUD::StaticIdType mapStaticId, Room *room)
{
    Q_UNUSED(obj)

    // Id
    int tmpId;
    if (!QMUD::fileJsonRead(obj, "id", tmpId))
        return Q_NULLPTR;

    // Name
    QString tmpName;
    if (!QMUD::fileJsonRead(obj, "name", tmpName))
        return Q_NULLPTR;

    tmpName = tmpName.simplified();

    // Sex
    QString tempString;
    if (!QMUD::fileJsonRead(obj, "sex", tempString, false, true, QMUD::sexTypeToString(QMUD::SexType::MALE)))
        return Q_NULLPTR;

    QMUD::SexType tmpSex = QMUD::sexTypeFromString(tempString);

    if (tmpSex == QMUD::SexType::UNKNOWN)
    {
        GLOBAL_CRITICAL(tr("Chiave 'sex' non valida: %1").arg(tempString));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return Q_NULLPTR;
    }

    // Description
    QStringList tmpDescription;
    if (!QMUD::fileJsonRead(obj, "description", tmpDescription, false, false, true))
        return Q_NULLPTR;

    // Race
    QString tmpRaceStr;
    if (!QMUD::fileJsonRead(obj, "race", tmpRaceStr))
        return Q_NULLPTR;

    bool ok;
    QMUD::RaceType tmpRace = QMUD::Race::fromString(tmpRaceStr, &ok);
    if (!ok)
    {
        GLOBAL_CRITICAL(tr("Chiave 'race' non valida: %1").arg(tmpRaceStr));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return Q_NULLPTR;
    }

    // NpcType
    QMUD::NpcType tmpNpcType;
    QString tmpNpcTypeStr;
    if (!QMUD::fileJsonRead(obj, "type", tmpNpcTypeStr, false, true, "normal"))
        return Q_NULLPTR;

    tmpNpcType = QMUD::npcTypeFromString(tmpNpcTypeStr);
    if (tmpNpcType == QMUD::NpcType::UNKNOWN)
    {
        GLOBAL_CRITICAL(tr("Chiave 'type' non valida: %1").arg(tmpNpcTypeStr));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return Q_NULLPTR;
    }

    // Attaccchi bonus
    int tmpNumberOfAttacksBonus = 0;

    if (!QMUD::fileJsonRead(obj, "numberOfAttacksBonus", tmpNumberOfAttacksBonus, true, 0))
        return Q_NULLPTR;

    // Resources
    QMap<QMUD::ResourceType, qint64> tmpResources;
    auto itResources = obj.find("resources");

    if (itResources != obj.end())
    {
        if (!itResources.value().isObject())
        {
            GLOBAL_CRITICAL(tr("Chiave 'resources' non valida"));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return Q_NULLPTR;
        }

        auto objResources = itResources->toObject();

        for (int i = static_cast<int>(QMUD::ResourceType::RESOURCE_FIRST);
             i < static_cast<int>(QMUD::ResourceType::MAX_VALUE); ++i)
        {
            auto itResource = objResources.find(QMUD::resourceToString(static_cast<QMUD::ResourceType>(i)));

            if (itResource != objResources.end() && itResource.value().isDouble())
                tmpResources[static_cast<QMUD::ResourceType>(i)] = qRound64(itResource.value().toDouble());
        }
    }

    // EventEnter
    QStringList tmpEventEnter;
    if (!QMUD::fileJsonRead(obj, "eventEnter", tmpEventEnter, false, false, true, true))
        return Q_NULLPTR;

    // EventCustom
    QStringList tmpEventCustom;
    if (!QMUD::fileJsonRead(obj, "eventCustom", tmpEventCustom, false, false, true, true))
        return Q_NULLPTR;

    // EventAbility
    QStringList tmpEventAbility;
    if (!QMUD::fileJsonRead(obj, "eventAbility", tmpEventAbility, false, false, true, true))
        return Q_NULLPTR;

    // Classe
    auto itClass = obj.find("class");
    auto itLevel = obj.find("level");

    if (itClass == obj.end())
    {
        GLOBAL_CRITICAL(tr("Chiave 'class' non trovata"));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return Q_NULLPTR;
    }

    if (!itClass->isString())
    {
        GLOBAL_CRITICAL(tr("Chiave 'class' non valida"));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return Q_NULLPTR;
    }

    if (itLevel == obj.end())
    {
        GLOBAL_CRITICAL(tr("Chiave 'level' non trovata"));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return Q_NULLPTR;
    }

    if (!itLevel->isDouble())
    {
        GLOBAL_CRITICAL(tr("Chiave 'level' non valida"));
        GLOBAL_CRITICAL_JSON_OBJ(obj);
        return Q_NULLPTR;
    }

    CharacterClass *chClass = CharacterClass::fromString(itClass->toString());
    int tmpLevel = itLevel->toInt();

    // Shop
    auto itShop = obj.find("shop");

    QVector<ShopItemData> shopItems;
    double shopSaleInflation = 1.0;
    double shopPurchaseInflation = 1.0;
    bool shopPurchaseBase = false;
    bool shopPurchaseArmor = false;
    bool shopPurchaseWeapon = false;

    if (itShop != obj.end())
    {
        if (!itShop->isObject())
        {
            GLOBAL_CRITICAL(tr("Chiave 'shop' non valida"));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return Q_NULLPTR;
        }

        auto objShop = itShop->toObject();
        auto itShopList = objShop.find("list");

        if (itShopList == objShop.end() || !itShopList->isArray())
        {
            GLOBAL_CRITICAL(tr("Chiave 'shop->list' non valida"));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return Q_NULLPTR;
        }

        auto arrayShopList = itShopList->toArray();

        for (auto item : arrayShopList)
        {
            ShopItemData itemData;
            itemData.id = static_cast<QMUD::StaticIdType>(qRound64(item.toDouble()));
            itemData.mapId = mapStaticId;
            shopItems.push_back(itemData);
        }

        if (arrayShopList.isEmpty())
        {
            GLOBAL_CRITICAL(tr("Chiave 'shop->list' non valida: non puo' essere vuota"));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return Q_NULLPTR;
        }

        // Inflazione di vendita
        if (!QMUD::fileJsonRead(obj, "saleInflation", shopSaleInflation, true, 1.0))
            return Q_NULLPTR;

        // Inflazione di acquisto
        if (!QMUD::fileJsonRead(obj, "purchaseInflation", shopPurchaseInflation, true, 1.0))
            return Q_NULLPTR;

        // Compra oggetti base
        if (!QMUD::fileJsonRead(obj, "purchaseBase", shopPurchaseBase, true, false))
            return Q_NULLPTR;

        // Compra armature
        if (!QMUD::fileJsonRead(obj, "purchaseArmor", shopPurchaseArmor, true, false))
            return Q_NULLPTR;

        // Compra armi
        if (!QMUD::fileJsonRead(obj, "purchaseWeapon", shopPurchaseWeapon, true, false))
            return Q_NULLPTR;
    }

    // Loot
    auto itLoot = obj.find("loot");
    QMap<QMUD::StaticIdType, LootItemData> loot;

    if (itLoot != obj.end())
    {
        if (!itLoot->isArray())
        {
            GLOBAL_CRITICAL(tr("Chiave 'loot' non valida"));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return Q_NULLPTR;
        }

        auto arrayLoot = itLoot->toArray();

        for (auto itemElem : arrayLoot)
        {
            if (!itemElem.isObject())
            {
                GLOBAL_CRITICAL(tr("Chiave 'loot' non valida"));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                return Q_NULLPTR;
            }

            QJsonObject objItem = itemElem.toObject();
            auto itemId = objItem.find("id");
            auto itemRarity = objItem.find("rarity");

            if (itemId == objItem.end() || !itemId->isDouble())
            {
                GLOBAL_CRITICAL(tr("Chiave 'loot' non valida, id non presente o non valido"));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                return Q_NULLPTR;
            }

            if (itemRarity != objItem.end() && !itemRarity->isString())
            {
                GLOBAL_CRITICAL(tr("Chiave 'loot' non valida, rarity non valida"));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                return Q_NULLPTR;
            }

            QMUD::ItemRarityType rarity = itemRarity != objItem.end() ? QMUD::itemRarityFromString(itemRarity->toString()) : QMUD::ItemRarityType::RARITY1;

            if (rarity == QMUD::ItemRarityType::UNKNOWN)
            {
                GLOBAL_CRITICAL(tr("Chiave 'loot' non valida, rarity '%1' non valida").arg(itemRarity->toString()));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                return Q_NULLPTR;
            }

            QMUD::StaticIdType staticId = static_cast<QMUD::StaticIdType>(qRound(itemId->toDouble()));

            auto itItem = objs.find(staticId);

            if (itItem == objs.end())
            {
                GLOBAL_CRITICAL(tr("Chiave loot non valida: id '%1' non valido").arg(staticId));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                return Q_NULLPTR;
            }

            QMUD::ItemQualityType quality = QMUD::ItemQualityType::JUNK;

            auto itQuality = itItem.value().find("quality");
            if (itQuality != itItem.value().end())
                quality = QMUD::itemQualityFromString(itQuality->toString());

            qint64 value = 0.0;
            auto itValue = itItem.value().find("value");
            if (itValue != itItem.value().end())
                value = qRound64(itValue->toDouble());

            auto itItemName = itItem.value().find("name");
            if (itItemName != itItem.value().end() && itItemName->isString())
            {
                LootItemData data(itItem.value(),
                    QString(),
                    mapStaticId,
                    quality,
                    rarity,
                    staticId,
                    itItemName->toString(),
                    false,
                    value);
                loot[staticId] = data;
            }
            else
            {
                GLOBAL_CRITICAL(tr("Chiave loot non valida: id '%1' corrispondente ad un oggetto non valido").arg(staticId));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                return Q_NULLPTR;
            }
        }
    }

    // Flag
    auto itFlags = obj.find("flags");
    QSet<QMUD::NpcCharacterFlagType> tmpFlags;

    if (itFlags != obj.end())
    {
        if (!itFlags->isArray())
        {
            GLOBAL_CRITICAL(tr("Chiave 'flags' non valida"));
            GLOBAL_CRITICAL_JSON_OBJ(obj);
            return Q_NULLPTR;
        }

        auto arrayFlags = itFlags->toArray();

        for (auto elem : arrayFlags)
        {
            if (!elem.isString())
            {
                GLOBAL_CRITICAL(tr("Chiave 'flags' non valida"));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                return Q_NULLPTR;
            }

            auto flag = QMUD::npcCharacterFlagTypeFromString(elem.toString());

            if (flag == QMUD::NpcCharacterFlagType::UNKNOWN)
            {
                GLOBAL_CRITICAL(tr("Chiave 'flags' non valida: flag '%1' non valida").arg(elem.toString()));
                GLOBAL_CRITICAL_JSON_OBJ(obj);
                return Q_NULLPTR;
            }

            if (flag == QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_LARGE)
            {
                if (tmpFlags.contains(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_SMALL))
                {
                    GLOBAL_CRITICAL(tr("Flag PATROL_WILD_ZONE_LARGE e PATROL_WILD_ZONE_SMALL non compatibili"));
                    GLOBAL_CRITICAL_JSON_OBJ(obj);
                    return Q_NULLPTR;
                }

                tmpFlags.insert(QMUD::NpcCharacterFlagType::PATROL);
            }
            else if (flag == QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_SMALL)
            {
                if (tmpFlags.contains(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_LARGE))
                {
                    GLOBAL_CRITICAL(tr("Flag PATROL_WILD_ZONE_SMALL e PATROL_WILD_ZONE_LARGE non compatibili"));
                    GLOBAL_CRITICAL_JSON_OBJ(obj);
                    return Q_NULLPTR;
                }

                tmpFlags.insert(QMUD::NpcCharacterFlagType::PATROL);
            }

            tmpFlags.insert(flag);
        }
    }

    // Assist group
    int tmpAssistGroup;
    if (!QMUD::fileJsonRead(obj, "assistGroup", tmpAssistGroup, true, 0))
        return Q_NULLPTR;

    // Creo l'NpcCharacter
    NpcCharacter *ch = new NpcCharacter(Q_NULLPTR);
    ch->m_eNpcType = tmpNpcType;

    ch->setParent(room);
    ch->setName(tmpName);
    ch->setSex(tmpSex);
    ch->setRace(tmpRace);
    ch->setChClass(chClass, tmpLevel, 0);

    #define QMUD_NPC_MIN_STATS_BY_CLASS_INT      0
    #define QMUD_NPC_MIN_STATS_BY_CLASS_WIS      1
    #define QMUD_NPC_MIN_STATS_BY_CLASS_CON      2
    #define QMUD_NPC_MIN_STATS_BY_CLASS_STR      3
    #define QMUD_NPC_MIN_STATS_BY_CLASS_DEX      4

    // INT, WIS, CON, STR, DEX, AC Bonus
     const int NpcMinStatsByClass[static_cast<int>(QMUD::ClassType::MAX_VALUE)][5] = {
        { 3,  3,    16,    18,         16 }, // CHCLASS_WARRIOR
        { 18, 18,   14,     3,          3 }, // CHCLASS_MAGE
        { 3,  3,    14,    18,         18 }, // CHCLASS_ROGUE
        { 18, 18,   14,     3,          3 }, // CHCLASS_PRIEST
        { 18, 18,   14,     3,          3 }, // CHCLASS_DRUID
        { 3,  3,    18,    18,         14 }, // CHCLASS_BARBARIAN
        { 3,  3,    14,    18,         18 }  // CHCLASS_MONK
    };

    int NpcItemStatBonusByClass[static_cast<int>(QMUD::ClassType::MAX_VALUE)][5] = {
        { 0, 0, Item::baseSecondaryStatBonus(tmpLevel), Item::basePrimaryStatBonus(tmpLevel), Item::baseTertiaryStatBonus(tmpLevel) }, // CHCLASS_WARRIOR
        { Item::basePrimaryStatBonus(tmpLevel), Item::baseSecondaryStatBonus(tmpLevel), Item::baseTertiaryStatBonus(tmpLevel), 0, 0 }, // CHCLASS_MAGE
        { 0, 0, Item::baseTertiaryStatBonus(tmpLevel), Item::baseSecondaryStatBonus(tmpLevel), Item::basePrimaryStatBonus(tmpLevel) }, // CHCLASS_ROGUE
        { Item::baseSecondaryStatBonus(tmpLevel), Item::basePrimaryStatBonus(tmpLevel), Item::baseTertiaryStatBonus(tmpLevel), 0, 0 }, // CHCLASS_PRIEST
        { Item::baseSecondaryStatBonus(tmpLevel), Item::basePrimaryStatBonus(tmpLevel), Item::baseTertiaryStatBonus(tmpLevel), 0, 0 }, // CHCLASS_DRUID
        { 0, 0, Item::baseSecondaryStatBonus(tmpLevel), Item::basePrimaryStatBonus(tmpLevel), Item::baseTertiaryStatBonus(tmpLevel) }, // CHCLASS_BARBARIAN
        { 0, 0, Item::baseTertiaryStatBonus(tmpLevel), Item::baseSecondaryStatBonus(tmpLevel), Item::basePrimaryStatBonus(tmpLevel) }  // CHCLASS_MONK
    };

    int classIndex = static_cast<int>(ch->chClassType());

    double statMult = 1.0;

    switch (ch->npcType())
    {
        case QMUD::NpcType::NORMAL:     statMult *= 0.5; break;
        case QMUD::NpcType::ELITE:      statMult *= 0.7; break;
        case QMUD::NpcType::RARE:       statMult *= 0.9; break;
        case QMUD::NpcType::LEGENDARY:  statMult *= 1.1; break;
        case QMUD::NpcType::BOSS_A:     statMult *= 1.2; break;
        case QMUD::NpcType::BOSS_B:     statMult *= 1.3; break;
    default:
        break;
    }

    ch->setStrengthBase    (qRound(qMax(NpcMinStatsByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_STR], QMUD::Race::baseNPCStr(tmpRace)) * statMult));
    ch->setConstitutionBase(qRound(qMax(NpcMinStatsByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_CON], QMUD::Race::baseNPCStr(tmpRace)) * statMult));
    ch->setIntelligenceBase(qRound(qMax(NpcMinStatsByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_INT], QMUD::Race::baseNPCStr(tmpRace)) * statMult));
    ch->setWisdomBase      (qRound(qMax(NpcMinStatsByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_WIS], QMUD::Race::baseNPCStr(tmpRace)) * statMult));
    ch->setDexterityBase   (qRound(qMax(NpcMinStatsByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_DEX], QMUD::Race::baseNPCStr(tmpRace)) * statMult));
    ch->setCharismaBase    (10);

    ch->m_iStrengthItemsBonus       = qRound(statMult * 13.0 * NpcItemStatBonusByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_STR] * baseStatMultiplier(tmpNpcType));
    ch->m_iConstitutionItemsBonus   = qRound(statMult * 13.0 * NpcItemStatBonusByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_CON] * baseStatMultiplier(tmpNpcType));
    ch->m_iIntelligenceItemsBonus   = qRound(statMult * 13.0 * NpcItemStatBonusByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_INT] * baseStatMultiplier(tmpNpcType));
    ch->m_iWisdomItemsBonus         = qRound(statMult * 13.0 * NpcItemStatBonusByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_WIS] * baseStatMultiplier(tmpNpcType));
    ch->m_iDexterityItemsBonus      = qRound(statMult * 13.0 * NpcItemStatBonusByClass[classIndex][QMUD_NPC_MIN_STATS_BY_CLASS_DEX] * baseStatMultiplier(tmpNpcType));

    if (tmpResources.find(QMUD::ResourceType::COINS) == tmpResources.end())
        tmpResources[QMUD::ResourceType::COINS] = baseCoins(ch->level(), tmpNpcType);
    ch->setResources(tmpResources);

    ch->m_iStaticId = static_cast<QMUD::StaticIdType>(tmpId);
    ch->m_iMapStaticId = mapStaticId;
    ch->m_iRoomStaticId = room->staticId();
    ch->m_strDescription = tmpDescription.join(QMUD_GLOBAL_NEW_LINE);
    ch->m_iGiveExperience = baseExperience(ch->level(), tmpNpcType);
    if (!tmpEventEnter.isEmpty())
        ch->m_mapEventFunction[QMUD::EventType::ENTER] = tmpEventEnter.join(QMUD_GLOBAL_NEW_LINE);
    if (!tmpEventCustom.isEmpty())
        ch->m_mapEventFunction[QMUD::EventType::CUSTOM] = tmpEventCustom.join(QMUD_GLOBAL_NEW_LINE);
    if (!tmpEventAbility.isEmpty())
        ch->m_mapEventFunction[QMUD::EventType::ABILITY] = tmpEventAbility.join(QMUD_GLOBAL_NEW_LINE);
    ch->m_mapLoot = loot;
    ch->m_vShopItems = shopItems;
    ch->m_dShopSaleInflation = shopSaleInflation;
    ch->m_dShopPurchaseInflation = shopPurchaseInflation;
    ch->m_bShopPurchaseBase = shopPurchaseBase;
    ch->m_bShopPurchaseArmor = shopPurchaseArmor;
    ch->m_bShopPurchaseWeapon = shopPurchaseWeapon;
    ch->m_vFlags = tmpFlags;
    ch->m_iAssistGroup = tmpAssistGroup;
    ch->m_iArmorClassBaseBonus = qRound(tmpLevel * 8 * NpcDataByType[static_cast<int>(tmpNpcType)]
            [static_cast<int>(chClass->classType())]
            [QMUD_NPC_DATA_BY_TYPE_AC_MULT]/2.0);
    ch->m_iNumberOfAttacksBonus = tmpNumberOfAttacksBonus;

    ch->chClass()->AIBuff(ch);

    ch->setHitPointsCurrent(ch->hitPointsCurrentMaximum());
    ch->setActionPointsCurrent(ch->actionPointsCurrentMaximum());
    ch->setManaPointsCurrent(ch->manaPointsCurrentMaximum());

    return ch;
}

void NpcCharacter::update()
{
    if (!isInCombat() || !targetCharacter(true))
    {
        for (auto it = m_mapHostiles.begin(); it != m_mapHostiles.end(); )
        {
            it.value()--;

            if (it.value() <= 0)
                it = m_mapHostiles.erase(it);
            else
                ++it;
        }

        bool startCombat = false;

        // Hunt
        if (m_iChHunted != QMUD::IdInvalid)
        {
            auto huntedCh = QMUD::qmudWorld->character(m_iChHunted);

            if (!huntedCh)
            {
                m_iChHuntedFailedTry++;
                m_iChHuntedSuccessedTry = 0;
            }
            else
            {
                bool currentRoom = false;

                if (canSee(huntedCh))
                {
                    RoomExit* exitToTake = hunt(huntedCh, true, currentRoom);

                    if (exitToTake)
                    {
                        if (exitToTake->isOpen())
                            Command::parse(exitToTake->roomExitToReadableString(), this)->execute();

                        if (this->room() != huntedCh->room())
                            Command::parse(tr("sdico Dove sei scappato %1?").arg(huntedCh->name(Q_NULLPTR)), this)->execute();
                        else
                        {
                            m_iChHuntedSuccessedTry = 0;
                            m_iChHuntedFailedTry = 0;
                            Command::parse(tr("sdico Ti ho trovato %1!").arg(huntedCh->name(Q_NULLPTR)), this)->execute();
                            Command::parse(tr("uccidi %1").arg(huntedCh->name(Q_NULLPTR)), this)->execute();
                            startCombat = true;
                        }

                        m_iChHuntedSuccessedTry++;
                        m_iChHuntedFailedTry = 0;
                    }
                    else if (currentRoom)
                    {
                        m_iChHuntedSuccessedTry = 0;
                        m_iChHuntedFailedTry = 0;
                    }
                    else
                    {
                        m_iChHuntedFailedTry++;
                        m_iChHuntedSuccessedTry = 0;
                    }
                }
                else
                {
                    m_iChHuntedFailedTry++;
                    m_iChHuntedSuccessedTry = 0;
                }
            }

            if (m_iChHuntedFailedTry > QMUD_GLOBAL_CH_NPC_MAX_HUNTING_FAILED_TRY ||
                m_iChHuntedSuccessedTry > QMUD_GLOBAL_CH_NPC_MAX_HUNTING_SUCCESSED_TRY)
            {
                stopHunt();
                setInCombat(false);

                auto map = QMUD::qmudWorld->map(mapStaticId());

                if (map)
                {
                    auto room = map->room(roomStaticId());

                    if (room)
                        setParent(room);
                }
            }
        }
        else if (flagCheck(QMUD::NpcCharacterFlagType::PATROL))
        {
            if (QMUD::rollDice(1, 5) == 1)
            {
                auto exits = room()->exits();

                if (!exits.isEmpty())
                {
                    auto exit = exits[QMUD::rollDice(1, exits.size()) - 1];

                    if (exit->exitRoom()->map()->staticId() == room()->map()->staticId() && !exit->isPatrolBarrier())
                    {
                        if (flagCheck(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_SMALL))
                        {
                            if (exit->exitRoom()->isWilderness() && room()->isWilderness())
                            {
                                Room* patrolRoom = QMUD::qmudWorld->map(m_iMapStaticId)->room(m_iRoomStaticId);

                                int currDistanceX = qAbs(room()->wildX() - patrolRoom->wildX());
                                int currDistanceY = qAbs(room()->wildY() - patrolRoom->wildY());
                                int newDistanceX = qAbs(exit->exitRoom()->wildX() - patrolRoom->wildX());
                                int newDistanceY = qAbs(exit->exitRoom()->wildY() - patrolRoom->wildY());

                                // Mi muovo solo se non mi allontano troppo dal centro dell'area di patrol oppure
                                // se muovendomi rientro nell'area di patrol
                                if ((newDistanceX <= QMUD_WILD_PATROL_WIDTH_RADIUS_SMALL && newDistanceY <= QMUD_WILD_PATROL_HEIGHT_RADIUS_SMALL) ||
                                     (newDistanceX <= currDistanceX && newDistanceY <= currDistanceY))
                                {
                                    // Mi muovo solo se rimango sullo stesso tipo di terreno
                                    if (exit->exitRoom()->flagCheck(QMUD::RoomFlagType::WATER) ==
                                        room()->flagCheck(QMUD::RoomFlagType::WATER))
                                        Command::parse(exit->roomExitToReadableString(), this)->execute();
                                }
                            }
                            else
                                Command::parse(exit->roomExitToReadableString(), this)->execute();
                        }
                        else if (flagCheck(QMUD::NpcCharacterFlagType::PATROL_WILD_ZONE_LARGE))
                        {
                            if (exit->exitRoom()->isWilderness() && room()->isWilderness())
                            {
                                Room* patrolRoom = QMUD::qmudWorld->map(m_iMapStaticId)->room(m_iRoomStaticId);

                                int currDistanceX = qAbs(room()->wildX() - patrolRoom->wildX());
                                int currDistanceY = qAbs(room()->wildY() - patrolRoom->wildY());
                                int newDistanceX = qAbs(exit->exitRoom()->wildX() - patrolRoom->wildX());
                                int newDistanceY = qAbs(exit->exitRoom()->wildY() - patrolRoom->wildY());

                                // Mi muovo solo se non mi allontano troppo dal centro dell'area di patrol oppure
                                // se muovendomi rientro nell'area di patrol
                                if ((newDistanceX <= QMUD_WILD_PATROL_WIDTH_RADIUS_LARGE && newDistanceY <= QMUD_WILD_PATROL_HEIGHT_RADIUS_LARGE) ||
                                     (newDistanceX <= currDistanceX && newDistanceY <= currDistanceY))
                                {
                                    // Mi muovo solo se rimango sullo stesso tipo di terreno
                                    if (exit->exitRoom()->flagCheck(QMUD::RoomFlagType::WATER) ==
                                        room()->flagCheck(QMUD::RoomFlagType::WATER))
                                        Command::parse(exit->roomExitToReadableString(), this)->execute();
                                }
                            }
                            else
                                Command::parse(exit->roomExitToReadableString(), this)->execute();
                        }
                        else
                            Command::parse(exit->roomExitToReadableString(), this)->execute();
                    }
                }
            }
        }

        // Gestione lista target ostili e aggressivita'
        if (!startCombat && !m_mapHostiles.isEmpty())
        {
            auto charsInRoom = room()->characters();

            for (auto ch : charsInRoom)
            {
                for (auto it = m_mapHostiles.begin(); it != m_mapHostiles.end(); ++it)
                {
                    if (ch->id() == it.key() && canSee(ch) && canAttack(ch))
                    {
                        Command::parse(QObject::tr("sdico Eccoti di nuovo!"), this)->execute();
                        Command::parse(QObject::tr("uccidi %1").arg(ch->name(Q_NULLPTR)), this)->execute();
                        startCombat = true;
                        break;
                    }
                }

                if (startCombat)
                    break;
            }
        }

        if (!startCombat && flagCheck(QMUD::NpcCharacterFlagType::AGGRESSIVE))
        {
            auto chInRoom = room()->characters();

            for (auto targetCh : chInRoom)
            {
                if ((targetCh->chType() == ChType::CHTYPE_NPC &&
                     static_cast<NpcCharacter*>(targetCh)->mapStaticId() != this->mapStaticId() &&
                     canSee(targetCh) && canAttack(targetCh))
                        ||
                    (targetCh->chType() == ChType::CHTYPE_PC &&
                     canSee(targetCh) && canAttack(targetCh)))
                {
                    Command::parse(QObject::tr("uccidi %1").arg(targetCh->name(Q_NULLPTR)), this)->execute();
                    break;
                }
            }
        }
    }

    Character::update();
}

double NpcCharacter::baseManaAndHpMultiplier(QMUD::NpcType type)
{
    switch (type)
    {
        case QMUD::NpcType::NORMAL:     return 0.7;
        case QMUD::NpcType::ELITE:      return 1.4;
        case QMUD::NpcType::RARE:       return 3.0;
        case QMUD::NpcType::LEGENDARY:  return 8.0;
        case QMUD::NpcType::BOSS_A:     return 16.0;
        case QMUD::NpcType::BOSS_B:     return 32.0;
    default:
        return 1;
    }
}

double NpcCharacter::baseStatMultiplier(QMUD::NpcType type)
{
    switch (type)
    {
        case QMUD::NpcType::NORMAL:     return 0.4;
        case QMUD::NpcType::ELITE:      return 0.8;
        case QMUD::NpcType::RARE:       return 1.0;
        case QMUD::NpcType::LEGENDARY:  return 1.3;
        case QMUD::NpcType::BOSS_A:     return 1.7;
        case QMUD::NpcType::BOSS_B:     return 1.9;
    default:
        return 1.0;
    }
}

qint64 NpcCharacter::baseCoins(int level, QMUD::NpcType type)
{
    int mult = 1;

    switch (type)
    {
        case QMUD::NpcType::NORMAL:     mult = 1; break;
        case QMUD::NpcType::ELITE:      mult = 2; break;
        case QMUD::NpcType::RARE:       mult = 4; break;
        case QMUD::NpcType::LEGENDARY:  mult = 8; break;
        case QMUD::NpcType::BOSS_A:     mult = 16; break;
        case QMUD::NpcType::BOSS_B:     mult = 32; break;
    default:
        break;
    }

    return qRound64(level * 5.0 * mult);
}

qint64 NpcCharacter::baseExperience(int level, QMUD::NpcType type)
{
    double multExp = 1.0;

    switch (type)
    {
        case QMUD::NpcType::NORMAL:     multExp = 1.0; break;
        case QMUD::NpcType::ELITE:      multExp = 1.5; break;
        case QMUD::NpcType::RARE:       multExp = 2.25; break;
        case QMUD::NpcType::LEGENDARY:  multExp = 3.375; break;
        case QMUD::NpcType::BOSS_A:     multExp = 5.0625; break;
        case QMUD::NpcType::BOSS_B:     multExp = 7.59375; break;
    default:
        break;
    }

    multExp /= 3.0;

    return qRound64((qPow(level, 4)/1000.0 + 100.0*level) * multExp);
}

QMUD::NpcType NpcCharacter::npcType() const
{
    return m_eNpcType;
}

QString NpcCharacter::description() const
{
    return m_strDescription;
}

qint64 NpcCharacter::giveExperience() const
{
    return m_iGiveExperience;
}

bool NpcCharacter::isToResurrect() const
{
    return m_bIsToResurrect;
}

void NpcCharacter::setToResurrect(bool v)
{
    m_bIsToResurrect = v;
}

QMUD::StaticIdType NpcCharacter::staticId() const
{
    return m_iStaticId;
}

QMUD::StaticIdType NpcCharacter::mapStaticId() const
{
    return m_iMapStaticId;
}

QMUD::StaticIdType NpcCharacter::roomStaticId() const
{
    return m_iRoomStaticId;
}

QMap<QMUD::EventType, QString> NpcCharacter::scripts() const
{
    return m_mapEventFunction;
}

bool NpcCharacter::flagCheck(QMUD::NpcCharacterFlagType flag) const
{
    return m_vFlags.contains(flag);
}

QSet<QMUD::NpcCharacterFlagType> NpcCharacter::flags() const
{
    return m_vFlags;
}

void NpcCharacter::eventCustom(QString event)
{
    QJSEngine *engine = qjsEngine(this);

    if (!engine)
        return;

    if (m_jsFunctionEventCustom.isCallable())
    {
        QJSValue ret = m_jsFunctionEventCustom.call(QJSValueList() << jsValue() << event);

        if (ret.isError())
            GLOBAL_CRITICAL(tr("CHSCRIPT_CUSTOM [ID:%1, MAPID:%2]:%3").arg(staticId()).arg(mapStaticId()).arg(ret.toString()));
    }
}

void NpcCharacter::eventEnter(Character *ch)
{
    QJSEngine *engine = qjsEngine(this);

    if (!engine)
        return;

    if (m_jsFunctionEventEnter.isCallable())
    {
        QJSValue ret = m_jsFunctionEventEnter.call(QJSValueList() << jsValue() << ch->jsValue());

        if (ret.isError())
            GLOBAL_CRITICAL(tr("CHSCRIPT_ENTER [ID:%1, MAPID:%2]:%3").arg(staticId()).arg(mapStaticId()).arg(ret.toString()));
    }
}

bool NpcCharacter::eventAbility(Character *ch)
{
    QJSEngine *engine = qjsEngine(this);

    if (!engine)
        return false;

    if (m_jsFunctionEventAbility.isCallable())
    {
        QJSValue ret = m_jsFunctionEventAbility.call(QJSValueList() << jsValue() << ch->jsValue());

        if (ret.isError())
            GLOBAL_CRITICAL(tr("CHSCRIPT_ABILITY [ID:%1, MAPID:%2]:%3").arg(staticId()).arg(mapStaticId()).arg(ret.toString()));
        else
            return ret.toBool();
    }

    return false;
}

void NpcCharacter::executeCommand(QString cmd)
{
    emit command(this, cmd);
}

void NpcCharacter::sendToRoom(QString message)
{
    auto characters = room()->characters();

    for (Character* chInRoom : characters)
    {
        if (chInRoom != this && chInRoom->chType() == Character::ChType::CHTYPE_PC)
        {
            Player *playerOther = static_cast<PcCharacter*>(chInRoom)->player();
            playerOther->sendLine(message.arg(name(chInRoom)));
        }
    }
}

QMap<QMUD::StaticIdType, NpcCharacter::LootItemData> NpcCharacter::loot() const
{
    return m_mapLoot;
}

bool NpcCharacter::isVendor() const
{
    return !m_vShopItems.isEmpty();
}

QVector<NpcCharacter::ShopItemData> NpcCharacter::shopItems() const
{
    return m_vShopItems;
}

double NpcCharacter::shopSaleInflation() const
{
    return m_dShopSaleInflation;
}

double NpcCharacter::shopPurchaseInflation() const
{
    return m_dShopPurchaseInflation;
}

bool NpcCharacter::shopPurchaseBase() const
{
    return m_bShopPurchaseBase;
}

bool NpcCharacter::shopPurchaseArmor() const
{
    return m_bShopPurchaseArmor;
}

bool NpcCharacter::shopPurchaseWeapon() const
{
    return m_bShopPurchaseWeapon;
}

void NpcCharacter::startHunt(Character *target)
{
    m_iChHunted = target->id();
    if (target->chType() == ChType::CHTYPE_PC)
        m_strChHuntedPcCharacterName = target->name(Q_NULLPTR);
    else
        m_strChHuntedPcCharacterName.clear();

    m_iChHuntedSuccessedTry = 0;
    m_iChHuntedFailedTry = 0;
}

void NpcCharacter::stopHunt()
{
    m_iChHunted = QMUD::IdInvalid;
    m_strChHuntedPcCharacterName.clear();
    m_iChHuntedFailedTry = 0;
    m_iChHuntedSuccessedTry = 0;
}

bool NpcCharacter::isHunting()
{
    return m_iChHunted != QMUD::IdInvalid;
}

int NpcCharacter::assistGroup() const
{
    return m_iAssistGroup;
}

QMap<QMUD::IdType, qint64> NpcCharacter::aggro() const
{
    return m_mapAggro;
}

qint64 NpcCharacter::aggro(Character *ch) const
{
    auto it = m_mapAggro.find(ch->id());

    if (it == m_mapAggro.end())
        return 0;
    else
        return it.value();
}

void NpcCharacter::clearAggro()
{
    m_mapAggro.clear();
}

void NpcCharacter::addAggro(Character *ch, qint64 value)
{
    m_mapAggro[ch->id()] += value;
}

void NpcCharacter::setAggro(Character *ch, qint64 value)
{
    m_mapAggro[ch->id()] = value;
}

Character *NpcCharacter::updateAggro()
{
    qint64 maxAggro = 0;
    Character* newTarget = Q_NULLPTR;

    auto currRoom = room();

    for (auto it = m_mapAggro.begin(); it != m_mapAggro.end(); )
    {
        Character* ch = QMUD::qmudWorld->character(it.key());

        if (!ch)
        {
            it = m_mapAggro.erase(it);
        }
        else
        {
            if (it.value() > maxAggro && ch->room() == currRoom)
            {
                maxAggro = it.value();
                newTarget = ch;
            }

            ++it;
        }
    }

    return newTarget;
}

void NpcCharacter::addHostileCh(QMUD::IdType id)
{
    m_mapHostiles[id] = 30;
}

QMap<QMUD::IdType, int> NpcCharacter::hostiles() const
{
    return m_mapHostiles;
}

bool NpcCharacter::rollForRandomLoot() const
{
    int perc = QMUD::npcTypeToLootPercentage(m_eNpcType);

    Q_ASSERT(perc >= 0 && perc <= 100);

    if (perc == 0)
        return false;

    return QMUD::rollDice(1, 100) <= perc;
}

QVector<Quest *> NpcCharacter::questsBeGived() const
{
    return m_vQuests;
}

void NpcCharacter::setQuestsBeGived(QVector<Quest *> quests)
{
    m_vQuests = quests;
}

RoomExit* NpcCharacter::hunt(Character* target, bool currentMapOnly, bool& isCurrRoom)
{
    static QVector<Room*> rooms;
    static bool reserved = false;

    isCurrRoom = false;

    if (!reserved)
    {
        reserved = true;
        rooms.reserve(QMUD_GLOBAL_CH_NPC_HUNTING_DISTANCE);
    }

    rooms.clear();

    Room* currRoom = room();
    QMUD::IdType targetRoomId = target->room()->id();

    if (currRoom->id() == targetRoomId)
    {
        GLOBAL_DEBUG("Target nella stanza");
        isCurrRoom = true;
        return Q_NULLPTR;
    }

    RoomExit* startRoomExitE = Q_NULLPTR;
    RoomExit* startRoomExitW = Q_NULLPTR;
    RoomExit* startRoomExitN = Q_NULLPTR;
    RoomExit* startRoomExitS = Q_NULLPTR;
    RoomExit* startRoomExitU = Q_NULLPTR;
    RoomExit* startRoomExitD = Q_NULLPTR;
    RoomExit* firstStep = Q_NULLPTR;

    rooms.push_back(currRoom);
    currRoom->setMark(currRoom->id());

    QMUD::StaticIdType currMapStaticId = currRoom->map()->staticId();

    std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();

    for (int i = 0; i < rooms.size(); ++i)
    {
        currRoom = rooms[i];

        auto exits = currRoom->exits();

        bool done = false;

        for (auto exit : exits)
        {
            if (!currentMapOnly || (exit->exitRoom()->map()->staticId() == currMapStaticId && !exit->isPatrolBarrier()))
            {
                Room* tmpRoom = exit->exitRoom();

                if (tmpRoom->currMark() == QMUD::IdInvalid)
                {
                    if (tmpRoom->id() == targetRoomId)
                    {
                        if (i == 0)
                        {
                            firstStep = exit;
                        }
                        else
                        {
                            if (startRoomExitE && startRoomExitE->exitRoom()->id() == currRoom->currMark())
                                firstStep = startRoomExitE;
                            else if (startRoomExitW && startRoomExitW->exitRoom()->id() == currRoom->currMark())
                                firstStep = startRoomExitW;
                            else if (startRoomExitN && startRoomExitN->exitRoom()->id() == currRoom->currMark())
                                firstStep = startRoomExitN;
                            else if (startRoomExitS && startRoomExitS->exitRoom()->id() == currRoom->currMark())
                                firstStep = startRoomExitS;
                            else if (startRoomExitU && startRoomExitU->exitRoom()->id() == currRoom->currMark())
                                firstStep = startRoomExitU;
                            else if (startRoomExitD && startRoomExitD->exitRoom()->id() == currRoom->currMark())
                                firstStep = startRoomExitD;
                        }

                        done = true;
                        break;
                    }
                    else
                    {
                        if (i == 0)
                        {
                            tmpRoom->setMark(tmpRoom->id());

                            if (exit->exitType() == RoomExit::RoomExitType::EXIT_NORTH)
                                startRoomExitN = exit;
                            else if (exit->exitType() == RoomExit::RoomExitType::EXIT_SOUTH)
                                startRoomExitS = exit;
                            else if (exit->exitType() == RoomExit::RoomExitType::EXIT_WEST)
                                startRoomExitW = exit;
                            else if (exit->exitType() == RoomExit::RoomExitType::EXIT_EAST)
                                startRoomExitE = exit;
                            else if (exit->exitType() == RoomExit::RoomExitType::EXIT_UP)
                                startRoomExitU = exit;
                            else if (exit->exitType() == RoomExit::RoomExitType::EXIT_DOWN)
                                startRoomExitD = exit;
                        }
                        else
                        {
                            tmpRoom->setMark(currRoom->currMark());
                        }

                        if (rooms.size() < QMUD_GLOBAL_CH_NPC_HUNTING_DISTANCE)
                            rooms.push_back(tmpRoom);
                    }
                }
            }
        }

        if (done)
            break;
    }

    for (auto room : rooms)
        room->setMark(QMUD::IdInvalid);

    GLOBAL_DEBUG("Target " + QString::number(targetRoomId) +
                 " Rooms " + QString::number(rooms.size()) +
                 " Tempo totale " + QString::number(static_cast<double>((std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::nanoseconds>(time.time_since_epoch()).count()))));

    return firstStep;
}

NpcCharacter::NpcCharacter(Room *parent) :
    Character(Character::ChType::CHTYPE_NPC, parent),
    m_iStaticId(QMUD::StaticIdInvalid),
    m_iMapStaticId(QMUD::StaticIdInvalid),
    m_iRoomStaticId(QMUD::StaticIdInvalid),
    m_bIsToResurrect(false),
    m_iGiveExperience(0),
    m_eNpcType(QMUD::NpcType::NORMAL),
    m_iConstitutionItemsBonus(0),
    m_iDexterityItemsBonus(0),
    m_iIntelligenceItemsBonus(0),
    m_iWisdomItemsBonus(0),
    m_iCharismaItemsBonus(0),
    m_iStrengthItemsBonus(0),
    m_iArmorClassBaseBonus(0),
    m_iNumberOfAttacksBonus(0),
    m_dShopSaleInflation(0.0),
    m_dShopPurchaseInflation(0.0),
    m_bShopPurchaseBase(false),
    m_bShopPurchaseArmor(false),
    m_bShopPurchaseWeapon(false),
    m_iChHunted(QMUD::IdInvalid),
    m_iChHuntedFailedTry(0),
    m_iChHuntedSuccessedTry(0),
    m_iAssistGroup(0)
{

}

void NpcCharacter::initJS()
{
    QJSEngine *engine = qjsEngine(this);

    auto itEnter = m_mapEventFunction.find(QMUD::EventType::ENTER);
    auto itCustom = m_mapEventFunction.find(QMUD::EventType::CUSTOM);
    auto itAbility = m_mapEventFunction.find(QMUD::EventType::ABILITY);

    if (itEnter != m_mapEventFunction.end() && !itEnter->isEmpty())
    {
        QString script = QString("( function (self, ch) { %1 } )").arg(*itEnter);
        m_jsFunctionEventEnter = engine->evaluate(script);

        if (m_jsFunctionEventEnter.isError())
            GLOBAL_CRITICAL(tr("CHSCRIPT_ENTER [ID:%1, MAPID:%2]:%3").arg(staticId()).arg(mapStaticId()).arg(m_jsFunctionEventEnter.toString()));
    }

    if (itCustom != m_mapEventFunction.end() && !itEnter->isEmpty())
    {
        QString script = QString("( function (self, event) { %1 } )").arg(*itCustom);
        m_jsFunctionEventCustom = engine->evaluate(script);

        if (m_jsFunctionEventCustom.isError())
            GLOBAL_CRITICAL(tr("CHSCRIPT_CUSTOM [ID:%1, MAPID:%2]:%3").arg(staticId()).arg(mapStaticId()).arg(m_jsFunctionEventCustom.toString()));
    }

    if (itAbility != m_mapEventFunction.end() && !itAbility->isEmpty())
    {
        QString script = QString("( function (self, ch) { %1 } )").arg(*itAbility);
        m_jsFunctionEventAbility = engine->evaluate(script);

        if (m_jsFunctionEventAbility.isError())
            GLOBAL_CRITICAL(tr("CHSCRIPT_ABILITY [ID:%1, MAPID:%2]:%3").arg(staticId()).arg(mapStaticId()).arg(m_jsFunctionEventAbility.toString()));
    }
}

int NpcCharacter::armorClassBaseBonus() const
{
    return m_iArmorClassBaseBonus;
}

int NpcCharacter::numberOfAttacksBonus() const
{
    return m_iNumberOfAttacksBonus;
}

int NpcCharacter::constitutionItemBonus() const
{
    return m_iConstitutionItemsBonus;
}

int NpcCharacter::dexterityItemBonus() const
{
    return m_iDexterityItemsBonus;
}

int NpcCharacter::intelligenceItemBonus() const
{
    return m_iIntelligenceItemsBonus;
}

int NpcCharacter::wisdomItemBonus() const
{
    return m_iWisdomItemsBonus;
}

int NpcCharacter::charismaItemBonus() const
{
    return m_iCharismaItemsBonus;
}

int NpcCharacter::strengthItemBonus() const
{
    return m_iStrengthItemsBonus;
}
