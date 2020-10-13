/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMWEAPON_H
#define ITEMWEAPON_H

#include "item.h"
#include "../global.h"

namespace CORE
{

class ItemWeapon : public Item
{
    Q_OBJECT
public:
    virtual ~ItemWeapon() Q_DECL_OVERRIDE;

    static Item* loadItem(const QJsonObject &obj, Entity* parent, int level, QMUD::ItemQualityType quality);
    static ItemWeapon* createFromRandomCode(QString randomCode, Entity *parent);
    static QString generateRandomCode(int level, QMUD::ItemClassType itemClass, QMUD::ItemQualityType quality);
    static bool fromRandomCodeToValues(QString randomCode, int& level, QString& name, QMUD::ItemClassType& classType, QMUD::ItemQualityType& quality);
    static bool isValidRandomCode(QString randomCode);
    virtual void writeJson(QJsonObject &obj) Q_DECL_OVERRIDE;

    QMUD::ItemClassType classType() const;

    QMUD::DamageType damageType() const;
    int damageMin() const;
    int damageMax() const;
    QMUD::WeaponType weaponType() const;

    int rollDamage() const;

    double speed() const;

    const QMUD::ItemBaseBonus& baseBonus() const Q_DECL_OVERRIDE;
    const QMUD::ItemBaseStatus& baseTemporaryStatus() const Q_DECL_OVERRIDE;

public slots:

private:
    explicit ItemWeapon(Entity *parent = Q_NULLPTR);

    static void getDamageMinAndMax(int level, QMUD::ItemQualityType quality, QMUD::WeaponType type, int& damageMin, int& damageMax);
    static void updateBaseBonus(int level, QMUD::ItemQualityType quality, QMUD::ItemClassType classType, QMUD::ItemBaseBonus& baseBonus);
    static QString getNameByRandomCode(QString randomCode);

    QMUD::DamageType m_eDamageType;
    int m_iDamageMin;
    int m_iDamageMax;
    QMUD::WeaponType m_eWeaponType;
    QMUD::ItemClassType m_eClassType;

    double m_dSpeed;

    QMUD::ItemBaseBonus m_itemBaseBonus;
    QMUD::ItemBaseStatus m_itemBaseTemporaryStatus;
};

} // namespace CORE

#endif // ITEMWEAPON_H
