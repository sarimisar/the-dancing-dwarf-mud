/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMARMOR_H
#define ITEMARMOR_H

#include "item.h"
#include "../global.h"

namespace CORE
{

class ItemArmor : public Item
{
    Q_OBJECT
public:  
    virtual ~ItemArmor() Q_DECL_OVERRIDE;

    static Item* loadItem(const QJsonObject &obj, Entity* parent, int level, QMUD::ItemQualityType quality);
    static QString generateRandomCode(int level, QMUD::ItemClassType itemClass, QMUD::ItemQualityType quality, QMUD::ArmorType armorType = QMUD::ArmorType::UNKNOWN);
    static ItemArmor* createFromRandomCode(QString randomCode, Entity *parent);
    static bool fromRandomCodeToValues(QString randomCode, int& level, QString& name, QMUD::ItemClassType& classType, QMUD::ItemQualityType& quality);
    static bool isValidRandomCode(QString randomCode);
    virtual void writeJson(QJsonObject &obj) Q_DECL_OVERRIDE;

    QMUD::ArmorType armorType() const;
    int armorClassBonus() const;

    QMUD::ItemClassType classType() const;

    const QMUD::ItemBaseBonus& baseBonus() const Q_DECL_OVERRIDE;
    const QMUD::ItemBaseStatus& baseTemporaryStatus() const Q_DECL_OVERRIDE;

    static int getArmorClassBonus(int level, QMUD::ItemQualityType quality, QMUD::ItemClassType classType);

public slots:

protected:
    static QString getNameByRandomCode(QString randomCode);

private:
    explicit ItemArmor(Entity *parent = Q_NULLPTR);

    static void updateBaseBonus(int level, QMUD::ItemQualityType quality, QMUD::ItemClassType classType, QMUD::ItemBaseBonus& baseBonus);

    QMUD::ArmorType m_eArmorType;
    int m_iArmorClassBonus;

    QMUD::ItemClassType m_eClassType;

    QMUD::ItemBaseBonus m_itemBaseBonus;
    QMUD::ItemBaseStatus m_itemBaseTemporaryStatus;
};

} // namespace CORE

#endif // ITEMARMOR_H
