/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMDRINK_H
#define ITEMDRINK_H

#include "item.h"
#include "../global.h"

namespace CORE
{

class ItemDrink : public Item
{
    Q_OBJECT
public:  
    virtual ~ItemDrink() Q_DECL_OVERRIDE;

    static Item* loadItem(const QJsonObject &obj, Entity* parent);
    virtual void writeJson(QJsonObject &obj) Q_DECL_OVERRIDE;

    int mpGainBonus() const;

public slots:

private:
    explicit ItemDrink(Entity *parent = Q_NULLPTR);

    int m_iMpGainBonus;
};

} // namespace CORE

#endif // ITEMDRINK_H
