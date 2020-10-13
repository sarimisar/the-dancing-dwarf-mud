/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "itemweapon.h"
#include "character.h"
#include "../global.h"
#include "../globaldebug.h"
#include "world.h"

#include <QJsonArray>

using namespace CORE;

ItemWeapon::ItemWeapon(Entity *parent) :
    Item(QMUD::ItemType::WEAPON, parent),
    m_eDamageType(QMUD::DamageType::SLASHING),
    m_iDamageMin(1),
    m_iDamageMax(1),
    m_eWeaponType(QMUD::WeaponType::SWORD),
    m_eClassType(QMUD::ItemClassType::TANK),
    m_dSpeed(1.0)
{

}

void ItemWeapon::getDamageMinAndMax(int level, QMUD::ItemQualityType quality, QMUD::WeaponType type, int &damageMin, int &damageMax)
{
    // Bonus per rarita'
    double qualityMultiplier = 0.6;
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:       qualityMultiplier = 0.6;  break;
        case QMUD::ItemQualityType::COMMON:     qualityMultiplier = 0.7;  break;
        case QMUD::ItemQualityType::UNCOMMON:   qualityMultiplier = 0.85; break;
        case QMUD::ItemQualityType::VALUABLE:   qualityMultiplier = 1.0;  break;
        case QMUD::ItemQualityType::RARE:       qualityMultiplier = 1.15; break;
        case QMUD::ItemQualityType::EPIC:       qualityMultiplier = 1.3;  break;
        case QMUD::ItemQualityType::LEGENDARY:  qualityMultiplier = 1.5;  break;
    }

    damageMin = qRound(((level - 1) * 1.875 + 4.5) * 1.4 * qualityMultiplier * QMUD::weaponTypeToDamageMultiplier(type));
    damageMax = qRound(((level - 1) * 1.875 + 4.5) * 2.0 * qualityMultiplier * QMUD::weaponTypeToDamageMultiplier(type));
}

void ItemWeapon::updateBaseBonus(int level, QMUD::ItemQualityType quality, QMUD::ItemClassType classType, QMUD::ItemBaseBonus &baseBonus)
{
    double statBonusMultiplier = 0.0;
    switch (quality)
    {
        case QMUD::ItemQualityType::JUNK:       statBonusMultiplier = 0.0;  break;
        case QMUD::ItemQualityType::COMMON:     statBonusMultiplier = 0.0;  break;
        case QMUD::ItemQualityType::UNCOMMON:   statBonusMultiplier = 0.5;  break;
        case QMUD::ItemQualityType::VALUABLE:   statBonusMultiplier = 0.8;  break;
        case QMUD::ItemQualityType::RARE:       statBonusMultiplier = 1.0;  break;
        case QMUD::ItemQualityType::EPIC:       statBonusMultiplier = 1.3;  break;
        case QMUD::ItemQualityType::LEGENDARY:  statBonusMultiplier = 1.6;  break;
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

    baseBonus.setIntelligence(baseBonus.intelligence() + qRound(statBonusInt * statBonusMultiplier));
    baseBonus.setWisdom(baseBonus.wisdom() + qRound(statBonusWis * statBonusMultiplier));
    baseBonus.setStrength(baseBonus.strength() + qRound(statBonusStr * statBonusMultiplier));
    baseBonus.setConstitution(baseBonus.constitution() + qRound(statBonusCon * statBonusMultiplier));
    baseBonus.setDexterity(baseBonus.dexterity() + qRound(statBonusDex * statBonusMultiplier));
}

QString ItemWeapon::getNameByRandomCode(QString randomCode)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return QString();

    //int level = elements[1].toInt();
    //QMUD::ItemClassType itemClass = QMUD::itemClassFromString(elements[2]);
    //QMUD::ItemQualityType quality = QMUD::itemQualityFromString(elements[3]);
    //int randomNumber0 = elements[4].toInt();
    QMUD::WeaponType weaponType = QMUD::weaponTypeFromString(elements[5]);

    QString name = "un'arma";

    switch (weaponType)
    {
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    case QMUD::WeaponType::CLUB:           // Clava
        name = "una clava"; break;
    case QMUD::WeaponType::DAGGER:         // Pugnale
        name = "un pugnale"; break;
    case QMUD::WeaponType::TWO_HAND_CLUB:  // Clava a due mani
        name = "una clava a due mani"; break;
    case QMUD::WeaponType::AXE:            // Ascia
        name = "un'ascia"; break;
    case QMUD::WeaponType::LIGHT_HAMMER:   // Martello
        name = "un martello"; break;
    case QMUD::WeaponType::MACE:           // Mazza
        name = "una mazza"; break;
    case QMUD::WeaponType::QUARTERSTAFF:   // Bastone
        name = "un bastone"; break;
    case QMUD::WeaponType::SICKLE:         // Falce
        name = "una falce"; break;
    case QMUD::WeaponType::TWO_HAND_AXE:   // Ascia a due mani
        name = "un'ascia a due mani"; break;
    case QMUD::WeaponType::HALBERD:        // Alabarda
        name = "una alabarda"; break;
    case QMUD::WeaponType::LANCE:          // Lancia
        name = "una lancia"; break;
    case QMUD::WeaponType::SHORT_SWORD:    // Spada corta
        name = "una spada corta"; break;
    case QMUD::WeaponType::SWORD:          // Spada
        name = "una spada"; break;
    case QMUD::WeaponType::LONG_SWORD:     // Spada lunga
        name = "una spada lunga"; break;
    case QMUD::WeaponType::MORNINGSTAR:    // Morningstar
        name = "una mazza"; break;
    case QMUD::WeaponType::PIKE:           // Picca
        name = "una picca"; break;
    case QMUD::WeaponType::RAPIER:         // Stocco
        name = "uno stocco"; break;
    case QMUD::WeaponType::SCIMITAR:       // Scimitarra
        name = "una scimitarra"; break;
    case QMUD::WeaponType::TRIDENT:        // Tridente
        name = "un tridente"; break;
    case QMUD::WeaponType::WAR_HAMMER:     // Martello da guerra
        name = "un martello da guerra"; break;
    case QMUD::WeaponType::KATANA:         // Katana
        name = "una katana";
        break;

    default:
        Q_ASSERT(Q_NULLPTR);
    }

    return name;
}

ItemWeapon::~ItemWeapon()
{
    GLOBAL_DEBUG_DESTRUCTOR("ItemWeapon", "Id: " + QString::number(id()) +
                            " StaticId: " + QString::number(staticId()) +
                            " MapId: " + QString::number(mapStaticId()));
}

Item *CORE::ItemWeapon::loadItem(const QJsonObject &obj, Entity *parent, int level, QMUD::ItemQualityType quality)
{
    // WeaponType
    QString tmpStr;
    QMUD::WeaponType tmpWeaponType;
    if (!QMUD::fileJsonRead(obj, "weaponType", tmpStr))
        return Q_NULLPTR;

    tmpStr = tmpStr.toLower();

    tmpWeaponType = QMUD::weaponTypeFromString(tmpStr);

    if (tmpWeaponType == QMUD::WeaponType::UNKNOWN)
    {
        GLOBAL_CRITICAL(tr("Chiave obbligatoria 'weaponType' non valida: %1").arg(tmpStr));
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

    ItemWeapon* item = new ItemWeapon(Q_NULLPTR);
    item->setParent(parent);
    item->m_eDamageType = QMUD::weaponTypeToDamageType(tmpWeaponType);
    getDamageMinAndMax(level, quality, tmpWeaponType, item->m_iDamageMin, item->m_iDamageMax);
    item->m_eWeaponType = tmpWeaponType;
    item->m_eClassType = tmpClassType;
    item->m_dSpeed = QMUD::weaponTypeToSpeedMultiplier(tmpWeaponType);
    item->m_itemBaseBonus = baseBonus;
    item->m_itemBaseTemporaryStatus = baseTemporaryStatus;

    return item;
}

ItemWeapon *ItemWeapon::createFromRandomCode(QString randomCode, Entity *parent)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return Q_NULLPTR;

    int level = elements[1].toInt();
    QMUD::ItemClassType itemClass = QMUD::itemClassFromString(elements[2]);
    QMUD::ItemQualityType quality = QMUD::itemQualityFromString(elements[3]);
    //int randomNumber0 = elements[4].toInt();
    QMUD::WeaponType weaponType = QMUD::weaponTypeFromString(elements[5]);

    QString name = getNameByRandomCode(randomCode);

    QMUD::ItemBaseBonus baseBonus;

    updateBaseBonus(level, quality, itemClass, baseBonus);

    ItemWeapon* item = new ItemWeapon(Q_NULLPTR);
    item->setParent(parent);
    item->m_iLevel = level;
    item->m_iCoinsValue = getDefaultCoinsValue(level, quality);
    item->m_iStaticId = QMUD_GLOBAL_STATIC_ID_ITEM_RANDOM_ITEM;
    item->m_iMapStaticId = QMUD_GLOBAL_MAP_STATIC_ID_ITEM_RANDOM_ITEM;
    item->m_strName = name;
    item->m_strDescription = name;
    item->m_eDamageType = QMUD::weaponTypeToDamageType(weaponType);
    getDamageMinAndMax(level, quality, weaponType, item->m_iDamageMin, item->m_iDamageMax);
    item->m_eWeaponType = weaponType;
    item->m_dSpeed = QMUD::weaponTypeToSpeedMultiplier(weaponType);
    item->m_eClassType = itemClass;
    item->m_itemBaseBonus = baseBonus;
    item->m_strRandomCode = randomCode;
    item->m_eQuality = quality;
    item->m_uuid = QUuid::createUuid();

    QMUD::qmudWorld->addItem(item);

    return item;
}

QString ItemWeapon::generateRandomCode(int level, QMUD::ItemClassType itemClass, QMUD::ItemQualityType quality)
{
    QMUD::WeaponType weaponType = static_cast<QMUD::WeaponType>(QMUD::rollDice(1, static_cast<int>(QMUD::WeaponType::MAX_VALUE) - 1));

    QString randomCode = "WEAPON-" + QString::number(level) + "-"
            + QMUD::itemClassToString(itemClass) + "-"
            + QMUD::itemQualityToString(quality) + "-"
            + QString::number(QMUD::rollDice(1, 5)) + "-"
            + QMUD::weaponTypeToString(weaponType);

    return randomCode;
}

bool ItemWeapon::fromRandomCodeToValues(QString randomCode, int &level, QString &name, QMUD::ItemClassType &classType, QMUD::ItemQualityType &quality)
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

bool ItemWeapon::isValidRandomCode(QString randomCode)
{
    QStringList elements = randomCode.split("-");

    if (elements.size() != 6)
        return false;

    return elements[0] == "WEAPON";
}

void ItemWeapon::writeJson(QJsonObject &obj)
{
    Item::writeJson(obj);

    if (!IsModify())
        return;

    obj["weaponType"] = QMUD::weaponTypeToString(m_eWeaponType);
    obj["classType"] = QMUD::itemClassToString(m_eClassType);

    QJsonObject objItemBaseBonus;
    m_itemBaseBonus.write(objItemBaseBonus);
    obj["baseBonus"] = objItemBaseBonus;

    QJsonObject objBaseStatus;
    m_itemBaseTemporaryStatus.write(objBaseStatus);
    obj["baseStatus"] = objBaseStatus;
}

QMUD::ItemClassType ItemWeapon::classType() const
{
    return m_eClassType;
}

QMUD::DamageType ItemWeapon::damageType() const
{
    return m_eDamageType;
}

int ItemWeapon::damageMin() const
{
    return m_iDamageMin;
}

int ItemWeapon::damageMax() const
{
    return m_iDamageMax;
}

QMUD::WeaponType ItemWeapon::weaponType() const
{
    return m_eWeaponType;
}

int ItemWeapon::rollDamage() const
{
    return m_iDamageMin + QMUD::rollDice(1, m_iDamageMax - m_iDamageMin);
}

double ItemWeapon::speed() const
{
    return m_dSpeed;
}

const QMUD::ItemBaseBonus &ItemWeapon::baseBonus() const
{
    return m_itemBaseBonus;
}

const QMUD::ItemBaseStatus &ItemWeapon::baseTemporaryStatus() const
{
    return m_itemBaseTemporaryStatus;
}
