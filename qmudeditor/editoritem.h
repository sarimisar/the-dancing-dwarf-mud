/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#ifndef EDITORITEM_H
#define EDITORITEM_H

#include <QObject>

#include "../common.h"

class EditorItem : public QObject
{
    Q_OBJECT
public:
    explicit EditorItem(QObject *parent = Q_NULLPTR);

    bool read(const QJsonObject& obj);
    bool write(QJsonObject& obj) const;

    int id() const;
    void setId(int id);

    int level() const;
    void setLevel(int level);

    QMUD::ItemQualityType quality() const;
    void setQuality(QMUD::ItemQualityType quality);

    QMUD::ItemType type() const;
    void setType(QMUD::ItemType type);

    QMUD::ItemClassType classType() const;
    void setClassType(QMUD::ItemClassType classType);

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    // Armor
    QMUD::ArmorType armorType() const;
    void setArmorType(QMUD::ArmorType armorType);

    // Weapon
    QMUD::WeaponType weaponType() const;
    void setWeaponType(QMUD::WeaponType weaponType);

    // Trinket
    QMUD::TrinketType trinketType() const;
    void setTrinketType(QMUD::TrinketType trinketType);

    // Drink
    int drinkMPGainBonus() const;
    void setDrinkMPGainBonus(int bonus);

    // Food
    int foodHPGainBonus() const;
    void setFoodHPGainBonus(int bonus);

    int foodAPGainBonus() const;
    void setFoodAPGainBonus(int bonus);

    // Potion
    int potionDurationSeconds() const;
    void setPotionDurationSeconds(int duration);

    int potionHP() const;
    void setPotionHP(int value);

    int potionMP() const;
    void setPotionMP(int value);

    int potionAP() const;
    void setPotionAP(int value);

    // Tonic
    int tonicDurationSeconds() const;
    void setTonicDurationSeconds(int duration);

    // Base Status
    QMUD::ItemBaseStatus baseStatus();
    void setBaseStatus(QMUD::ItemBaseStatus status);

    // Base Bonus
    QMUD::ItemBaseBonus baseBonus();
    void setBaseBonus(QMUD::ItemBaseBonus baseBonus);

private:
    int m_id;
    int m_iLevel;

    QMUD::ItemQualityType m_eQuality;
    QMUD::ItemType m_eType;
    QMUD::ItemClassType m_eClassType;

    QString m_name;
    QString m_strDescription;

    // Armature
    QMUD::ArmorType m_eArmorType;

    // Armi
    QMUD::WeaponType m_eWeaponType;

    // Gioielli
    QMUD::TrinketType m_eTrinketType;

    // Drink
    int m_iDrinkMPGainBonus;

    // Cibo
    int m_iFoodHPGainBonus;
    int m_iFoodAPGainBonus;

    // Pozioni
    int m_iPotionDurationSeconds;
    int m_iPotionHPGain;
    int m_iPotionMPGain;
    int m_iPotionAPGain;

    // Tonici
    int m_iTonicDurationSeconds;

    // Base Status
    QMUD::ItemBaseStatus m_baseStatus;

    // Base Bonus
    QMUD::ItemBaseBonus m_baseBonus;
};

Q_DECLARE_METATYPE(EditorItem*)

#endif // EDITORITEM_H
