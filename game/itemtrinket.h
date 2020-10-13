/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMTRINKET_H
#define ITEMTRINKET_H

#include "item.h"
#include "../global.h"

namespace CORE
{

class ItemTrinket : public Item
{
    Q_OBJECT
public:  
    virtual ~ItemTrinket() Q_DECL_OVERRIDE;

    static Item* loadItem(const QJsonObject &obj, Entity* parent, int level, QMUD::ItemQualityType quality);
    static ItemTrinket* createFromRandomCode(QString randomCode, Entity *parent);
    static QString generateRandomCode(int level, QMUD::ItemClassType itemClass, QMUD::ItemQualityType quality);
    static bool fromRandomCodeToValues(QString randomCode, int& level, QString& name, QMUD::ItemClassType& classType, QMUD::ItemQualityType& quality);
    static bool isValidRandomCode(QString randomCode);
    virtual void writeJson(QJsonObject &obj) Q_DECL_OVERRIDE;

    QMUD::TrinketType trinketType() const;

    QMUD::ItemClassType classType() const;

    const QMUD::ItemBaseBonus& baseBonus() const Q_DECL_OVERRIDE;
    const QMUD::ItemBaseStatus& baseTemporaryStatus() const Q_DECL_OVERRIDE;

public slots:

private:
    explicit ItemTrinket(Entity *parent = Q_NULLPTR);

    static void updateBaseBonus(int level, QMUD::ItemQualityType quality, QMUD::ItemClassType classType, QMUD::ItemBaseBonus& baseBonus);
    static QString getNameByRandomCode(QString randomCode);

    QMUD::TrinketType m_eTrinketType;
    QMUD::ItemClassType m_eClassType;

    QMUD::ItemBaseBonus m_itemBaseBonus;
    QMUD::ItemBaseStatus m_itemBaseTemporaryStatus;
};

} // namespace CORE

#endif // ITEMTRINKET_H
