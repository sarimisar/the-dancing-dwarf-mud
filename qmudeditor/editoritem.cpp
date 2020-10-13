/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "editoritem.h"

#include <QJsonArray>


EditorItem::EditorItem(QObject *parent) :
    QObject(parent),
    m_id(0),
    m_iLevel(1),
    m_eQuality(QMUD::ItemQualityType::JUNK),
    m_eType(QMUD::ItemType::BASE),
    m_eClassType(QMUD::ItemClassType::TANK),
    m_name(tr("Nuovo Oggetto")),
    m_eArmorType(QMUD::ArmorType::CHEST),
    m_eWeaponType(QMUD::WeaponType::SWORD),
    m_eTrinketType(QMUD::TrinketType::RING),
    m_iDrinkMPGainBonus(50),
    m_iFoodHPGainBonus(50),
    m_iFoodAPGainBonus(50),
    m_iPotionDurationSeconds(0),
    m_iPotionHPGain(0),
    m_iPotionMPGain(0),
    m_iPotionAPGain(0),
    m_iTonicDurationSeconds(3600)
{

}

bool EditorItem::read(const QJsonObject &obj)
{
    m_id = obj["id"].toInt();
    m_name = obj["name"].toString();
    m_iLevel = obj["level"].toInt();
    m_eQuality = QMUD::itemQualityFromString(obj["quality"].toString());
    m_eType = QMUD::itemTypeFromString(obj["type"].toString());
    m_eClassType = QMUD::itemClassFromString(obj["classType"].toString());
    if (m_eClassType == QMUD::ItemClassType::UNKNOWN)
        m_eClassType = QMUD::ItemClassType::DPS_STR;

    if (obj.contains("description"))
        m_strDescription =  obj["description"].toVariant().toStringList().join("\n");

    if (obj.contains("armorType"))
        m_eArmorType = QMUD::armorTypeFromString(obj["armorType"].toString());

    if (obj.contains("weaponType"))
        m_eWeaponType = QMUD::weaponTypeFromString(obj["weaponType"].toString());

    if (obj.contains("trinketType"))
        m_eTrinketType = QMUD::trinketTypeFromString(obj["trinketType"].toString());

    if (obj.contains("mpGainBonus"))
        m_iDrinkMPGainBonus = obj["mpGainBonus"].toInt();

    if (obj.contains("hpGainBonus"))
        m_iFoodHPGainBonus = obj["hpGainBonus"].toInt();

    if (obj.contains("apGainBonus"))
        m_iFoodAPGainBonus = obj["apGainBonus"].toInt();

    if (obj.contains("durationSeconds"))
    {
        m_iPotionDurationSeconds = obj["durationSeconds"].toInt();
        m_iTonicDurationSeconds = obj["durationSeconds"].toInt();
    }

    if (obj.contains("hitPoints"))
        m_iPotionHPGain = obj["hitPoints"].toInt();

    if (obj.contains("manaPoints"))
        m_iPotionMPGain = obj["manaPoints"].toInt();

    if (obj.contains("actionPoints"))
        m_iPotionAPGain = obj["actionPoints"].toInt();

    if (obj.contains("baseStatus"))
        m_baseStatus.load(obj["baseStatus"].toObject());

    if (obj.contains("baseBonus"))
        m_baseBonus.load(obj["baseBonus"].toObject());

    return true;
}

bool EditorItem::write(QJsonObject &obj) const
{
    obj["id"] = m_id;
    obj["name"] = m_name;
    obj["level"] = m_iLevel;
    obj["quality"] = QMUD::itemQualityToString(m_eQuality);
    obj["type"] = QMUD::itemTypeToString(m_eType);
    obj["classType"] = QMUD::itemClassToString(m_eClassType);

    if (!m_strDescription.isEmpty())
        obj["description"] = QJsonArray::fromStringList(m_strDescription.split("\n"));

    if (m_eType == QMUD::ItemType::ARMOR)
    {
        obj["armorType"] = QMUD::armorTypeToString(m_eArmorType);
    }
    else if (m_eType == QMUD::ItemType::WEAPON)
    {
        obj["weaponType"] = QMUD::weaponTypeToString(m_eWeaponType);
    }
    else if (m_eType == QMUD::ItemType::TRINKET)
    {
        obj["trinketType"] = QMUD::trinketTypeToString(m_eTrinketType);
    }
    else if (m_eType == QMUD::ItemType::DRINK)
    {
        obj["mpGainBonus"] = m_iDrinkMPGainBonus;
    }
    else if (m_eType == QMUD::ItemType::FOOD)
    {
        obj["hpGainBonus"] = m_iFoodHPGainBonus;
        obj["apGainBonus"] = m_iFoodAPGainBonus;
    }
    else if (m_eType == QMUD::ItemType::POTION)
    {
        obj["hitPoints"] = m_iPotionHPGain;
        obj["manaPoints"] = m_iPotionMPGain;
        obj["actionPoints"] = m_iPotionAPGain;
        obj["durationSeconds"] = m_iPotionDurationSeconds;
    }
    else if (m_eType == QMUD::ItemType::TONIC)
    {
        obj["durationSeconds"] = m_iTonicDurationSeconds;
    }

    if (m_baseStatus.isValid())
    {
        QJsonObject objBaseStatus;
        m_baseStatus.write(objBaseStatus);
        obj["baseStatus"] = objBaseStatus;
    }

    if (m_baseBonus.isValid())
    {
        QJsonObject objBaseBonus;
        m_baseBonus.write(objBaseBonus);
        obj["baseBonus"] = objBaseBonus;
    }

    return true;
}

int EditorItem::id() const
{
    return m_id;
}

void EditorItem::setId(int id)
{
    m_id = id;
}

int EditorItem::level() const
{
    return m_iLevel;
}

void EditorItem::setLevel(int level)
{
    m_iLevel = qMax(1, level);
}

QMUD::ItemQualityType EditorItem::quality() const
{
    return m_eQuality;
}

void EditorItem::setQuality(QMUD::ItemQualityType quality)
{
    m_eQuality = quality;
}

QMUD::ItemType EditorItem::type() const
{
    return m_eType;
}

void EditorItem::setType(QMUD::ItemType type)
{
    m_eType = type;
}

QString EditorItem::name() const
{
    return m_name;
}

void EditorItem::setName(const QString &name)
{
    m_name = name;
}

QString EditorItem::description() const
{
    return m_strDescription;
}

void EditorItem::setDescription(const QString &description)
{
    m_strDescription = description;
}

QMUD::ArmorType EditorItem::armorType() const
{
    return m_eArmorType;
}

void EditorItem::setArmorType(QMUD::ArmorType armorType)
{
    m_eArmorType = armorType;
}

QMUD::WeaponType EditorItem::weaponType() const
{
    return m_eWeaponType;
}

void EditorItem::setWeaponType(QMUD::WeaponType weaponType)
{
    m_eWeaponType = weaponType;
}

QMUD::TrinketType EditorItem::trinketType() const
{
    return m_eTrinketType;
}

void EditorItem::setTrinketType(QMUD::TrinketType trinketType)
{
    m_eTrinketType = trinketType;
}

int EditorItem::drinkMPGainBonus() const
{
    return m_iDrinkMPGainBonus;
}

void EditorItem::setDrinkMPGainBonus(int bonus)
{
    m_iDrinkMPGainBonus = bonus;
}

int EditorItem::foodHPGainBonus() const
{
    return m_iFoodHPGainBonus;
}

void EditorItem::setFoodHPGainBonus(int bonus)
{
    m_iFoodHPGainBonus = bonus;
}

int EditorItem::foodAPGainBonus() const
{
    return m_iFoodAPGainBonus;
}

void EditorItem::setFoodAPGainBonus(int bonus)
{
    m_iFoodAPGainBonus = bonus;
}

int EditorItem::potionDurationSeconds() const
{
    return m_iPotionDurationSeconds;
}

void EditorItem::setPotionDurationSeconds(int duration)
{
    m_iPotionDurationSeconds = duration;
}

int EditorItem::potionHP() const
{
    return m_iPotionHPGain;
}

void EditorItem::setPotionHP(int value)
{
    m_iPotionHPGain = value;
}

int EditorItem::potionMP() const
{
    return m_iPotionMPGain;
}

void EditorItem::setPotionMP(int value)
{
    m_iPotionMPGain = value;
}

int EditorItem::potionAP() const
{
    return m_iPotionAPGain;
}

void EditorItem::setPotionAP(int value)
{
    m_iPotionAPGain = value;
}

int EditorItem::tonicDurationSeconds() const
{
    return m_iTonicDurationSeconds;
}

void EditorItem::setTonicDurationSeconds(int duration)
{
    m_iTonicDurationSeconds = duration;
}

QMUD::ItemBaseStatus EditorItem::baseStatus()
{
    return m_baseStatus;
}

void EditorItem::setBaseStatus(QMUD::ItemBaseStatus status)
{
    m_baseStatus = status;
}

QMUD::ItemBaseBonus EditorItem::baseBonus()
{
    return m_baseBonus;
}

void EditorItem::setBaseBonus(QMUD::ItemBaseBonus baseBonus)
{
    m_baseBonus = baseBonus;
}

QMUD::ItemClassType EditorItem::classType() const
{
    return m_eClassType;
}

void EditorItem::setClassType(QMUD::ItemClassType classType)
{
    m_eClassType = classType;
}

