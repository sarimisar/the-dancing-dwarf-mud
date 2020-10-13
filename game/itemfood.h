/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMFOOD_H
#define ITEMFOOD_H

#include "item.h"
#include "../global.h"

namespace CORE
{

class ItemFood : public Item
{
    Q_OBJECT
public:  
    virtual ~ItemFood() Q_DECL_OVERRIDE;

    static Item* loadItem(const QJsonObject &obj, Entity* parent);
    virtual void writeJson(QJsonObject &obj) Q_DECL_OVERRIDE;

    int hpGainBonus() const;
    int apGainBonus() const;

public slots:

private:
    explicit ItemFood(Entity *parent = Q_NULLPTR);

    int m_iHpGainBonus;
    int m_iApGainBonus;
};

} // namespace CORE

#endif // ITEMFOOD_H
