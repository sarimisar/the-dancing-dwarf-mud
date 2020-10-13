/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemtrinket.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"
#include "world.h"

#include <QJsonArray>

using namespace CORE;

ItemTrinket::ItemTrinket(Entity *parent) :
    Item(QMUD::ItemType::TRINKET, parent),
    m_eTrinketType(QMUD::TrinketType::RING),
    m_eClassType(QMUD::ItemClassType::TANK)
{

}

ItemTrinket::~ItemTrinket()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemTrinket", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

Item *CORE::ItemTrinket::loadItem(const QJsonObject &obj, Entity *parent, int level, QMUD::ItemQualityType quality)
{
    // TrinketType
    QString tmpStr;
    if (!QMUD::fileJsonRead(obj, "trinketType", tmpStr))
        return Q_NULLPTR;

    QMUD::TrinketType tmpTrinketType = QMUD::trinketTypeFromString(tmpStr);

    if (tmpTrinketType == QMUD::TrinketType::UNKNOWN)
    {
        GLOBAL_CRITICAL(tr("Chiave obbligatoria 'trinketType' non valida: %1").arg(tmpStr));
        return Q_NULLPTR;
    }

    // ItemClassType
    QMUD::ItemClassType tmpClassType;
    if (!QMUD::fileJsonRead(obj, "classType", tmpStr))
        return Q_NULLPTR;

    tmpClassType = QMUD::itemClassFromString(tmpStr);

    if (tmpClassType == QMUD::ItemClassType::UNKNOWN)
    {
        GLOBAL_CRITICAL(tr("Chiave obbligatoria 'classType' non valida: %1").arg(tmpStr));
        return Q_NULLPTR;
    }

    // Base bonus
    QMUD::ItemBaseBonus baseBonus;
    auto itBaseBonus = obj.find("baseBonus");
    if (itBaseBonus != obj.end() && itBaseBonus->isObject())
    {
        if (!baseBonus.load(itBaseBonus->toObject()))
        {
            GLOBAL_CRITICAL(tr("Chiave 'baseBonus' non valida"));
            return Q_NULLPTR;
        }
    }

    // Base temporary status
    QMUD::ItemBaseStatus baseTemporaryStatus;
    auto itBaseTemporaryStatus = obj.find("baseStatus");
    if (itBaseTemporaryStatus != obj.end() && itBaseTemporaryStatus->isObject())
    {
        if (!baseTemporaryStatus.load(itBaseTemporaryStatus->toObject()))
        {
            GLOBAL_CRITICAL(tr("Chiave 'baseStatus' non valida"));
            return Q_NULLPTR;
        }
    }

    updateBaseBonus(level, quality, tmpClassType, baseBonus);

    ItemTrinket* item = new ItemTrinket(Q_NULLPTR);
    item->setParent(parent);
    item->m_eTrinketType = tmpTrinketType;
    item->m_eClassType = tmpClassType;
    item->m_itemBaseBonus = baseBonus;
    item->m_itemBaseTemporaryStatus = baseTemporaryStatus;

    return item;
}

ItemTrinket *ItemTrinket::createFromRandomCode(QString randomCode, Entity *parent)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return Q_NULLPTR;

    int level = elements[1].toInt();
    QMUD::ItemClassType itemClass = QMUD::itemClassFromString(elements[2]);
    QMUD::ItemQualityType quality = QMUD::itemQualityFromString(elements[3]);
    //int randomNumber0 = elements[4].toInt();
    QMUD::TrinketType trinketType = QMUD::trinketTypeFromString(elements[5]);

    QString name = getNameByRandomCode(randomCode);

    QMUD::ItemBaseBonus baseBonus;

    updateBaseBonus(level, quality, itemClass, baseBonus);

    ItemTrinket* item = new ItemTrinket(Q_NULLPTR);
    item->setParent(parent);
    item->m_iLevel = level;
    item->m_iCoinsValue = getDefaultCoinsValue(level, quality);
    item->m_iStaticId = QMUD_GLOBAL_STATIC_ID_ITEM_RANDOM_ITEM;
    item->m_iMapStaticId = QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RANDOM_ITEM;
    item->m_strName = name;
    item->m_strDescription = name;
    item->m_eTrinketType = trinketType;
    item->m_eClassType = itemClass;
    item->m_itemBaseBonus = baseBonus;
    item->m_strRandomCode = randomCode;
    item->m_eQuality = quality;
    item->m_uuid = QUuid::createUuid();

    QMUD::qmudWorld->addItem(item);

    return item;
}

QString ItemTrinket::generateRandomCode(int level, QMUD::ItemClassType itemClass, QMUD::ItemQualityType quality)
{
    QMUD::TrinketType trinketType = static_cast<QMUD::TrinketType>(QMUD::rollDice(1, static_cast<int>(QMUD::TrinketType::MAX_VALUE) - 1));

    QString randomCode = "TRINKET-" + QString::number(level) + "-"
            + QMUD::itemClassToString(itemClass) + "-"
            + QMUD::itemQualityToString(quality) + "-"
            + QString::number(QMUD::rollDice(1, 5)) + "-"
            + QMUD::trinketTypeToString(trinketType);

    return randomCode;
}

bool ItemTrinket::fromRandomCodeToValues(QString randomCode, int &level, QString &name, QMUD::ItemClassType &classType, QMUD::ItemQualityType &quality)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return false;

    level = elements[1].toInt();
    classType = QMUD::itemClassFromString(elements[2]);
    quality = QMUD::itemQualityFromString(elements[3]);

    name = getNameByRandomCode(randomCode);

    return true;
}

bool ItemTrinket::isValidRandomCode(QString randomCode)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return false;

    return elements[0] == "TRINKET";
}

void ItemTrinket::writeJson(QJsonObject &obj)
{
    Item::writeJson(obj);

    if (!IsModify())
        return;

    obj["trinketType"] = QMUD::trinketTypeToString(m_eTrinketType);
    obj["classType"] = QMUD::itemClassToString(m_eClassType);

    QJsonObject objItemEquippedData;
    m_itemBaseBonus.write(objItemEquippedData);
    obj["baseBonus"] = objItemEquippedData;

    QJsonObject objBaseStatus;
    m_itemBaseTemporaryStatus.write(objBaseStatus);
    obj["baseStatus"] = objBaseStatus;
}

QMUD::TrinketType ItemTrinket::trinketType() const
{
    return m_eTrinketType;
}

QMUD::ItemClassType ItemTrinket::classType() const
{
    return m_eClassType;
}

const QMUD::ItemBaseBonus &ItemTrinket::baseBonus() const
{
    return m_itemBaseBonus;
}

const QMUD::ItemBaseStatus &ItemTrinket::baseTemporaryStatus() const
{
    return m_itemBaseTemporaryStatus;
}

void ItemTrinket::updateBaseBonus(int level, QMUD::ItemQualityType quality, QMUD::ItemClassType classType, QMUD::ItemBaseBonus &baseBonus)
{
    double statBonusMultiplier = 0.0;
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:       statBonusMultiplier = 0.0;  break;
        case QMUD::ItemQualityType::COMMON:     statBonusMultiplier = 0.0;  break;
        case QMUD::ItemQualityType::UNCOMMON:   statBonusMultiplier = 0.4;  break;
        case QMUD::ItemQualityType::VALUABLE:   statBonusMultiplier = 0.6;  break;
        case QMUD::ItemQualityType::RARE:       statBonusMultiplier = 0.8;  break;
        case QMUD::ItemQualityType::EPIC:       statBonusMultiplier = 1.0;  break;
        case QMUD::ItemQualityType::LEGENDARY:  statBonusMultiplier = 1.3;  break;
    }

    int statBonusInt = 0;
    int statBonusWis = 0;
    int statBonusStr = 0;
    int statBonusCon = 0;
    int statBonusDex = 0;

    switch (classType)
    {
    case QMUD::ItemClassType::CASTER_INT:
        statBonusInt = qMax(0, basePrimaryStatBonus(level));
        statBonusWis = qMax(0, baseSecondaryStatBonus(level));
        statBonusCon = qMax(0, baseTertiaryStatBonus(level));
    break;

    case QMUD::ItemClassType::CASTER_WIS:
        statBonusWis = qMax(0, basePrimaryStatBonus(level));
        statBonusInt = qMax(0, baseSecondaryStatBonus(level));
        statBonusCon = qMax(0, baseTertiaryStatBonus(level));
    break;

    case QMUD::ItemClassType::DPS_DEX:
        statBonusDex = qMax(0, basePrimaryStatBonus(level));
        statBonusStr = qMax(0, baseSecondaryStatBonus(level));
        statBonusCon = qMax(0, baseTertiaryStatBonus(level));
    break;

    case QMUD::ItemClassType::DPS_STR:
        statBonusStr = qMax(0, basePrimaryStatBonus(level));
        statBonusCon = qMax(0, baseSecondaryStatBonus(level));
        statBonusDex = qMax(0, baseTertiaryStatBonus(level));
    break;

    case QMUD::ItemClassType::TANK:
        statBonusCon = qMax(0, basePrimaryStatBonus(level));
        statBonusStr = qMax(0, baseSecondaryStatBonus(level));
        statBonusDex = qMax(0, baseTertiaryStatBonus(level));
    break;

    default:
        break;
    }

    baseBonus.setIntelligence(qRound(statBonusInt * statBonusMultiplier));
    baseBonus.setWisdom(qRound(statBonusWis * statBonusMultiplier));
    baseBonus.setStrength(qRound(statBonusStr * statBonusMultiplier));
    baseBonus.setConstitution(qRound(statBonusCon * statBonusMultiplier));
    baseBonus.setDexterity(qRound(statBonusDex * statBonusMultiplier));
}

QString ItemTrinket::getNameByRandomCode(QString randomCode)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return QString();

    //int level = elements[1].toInt();
    //QMUD::ItemClassType itemClass = QMUD::itemClassFromString(elements[2]);
    //QMUD::ItemQualityType quality = QMUD::itemQualityFromString(elements[3]);
    //int randomNumber0 = elements[4].toInt();
    QMUD::TrinketType trinketType = QMUD::trinketTypeFromString(elements[5]);


    QString name = "un gioiello";

    switch (trinketType)
    {
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    case QMUD::TrinketType::RING:
        name = "un anello";
        break;
    case QMUD::TrinketType::NECKLACE:
        name = "una collana";
        break;
    case QMUD::TrinketType::BRACELET:
        name = "dei bracciali";
        break;

    default:
        Q_ASSERT(Q_NULLPTR);
    }

    return name;
}
