/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMTONIC_H
#define ITEMTONIC_H

#include "item.h"
#include "../global.h"

namespace CORE
{

class ItemTonic : public Item
{
    Q_OBJECT
public:  
    virtual ~ItemTonic() Q_DECL_OVERRIDE;

    static Item* loadItem(const QJsonObject &obj, Entity* parent);
    virtual void writeJson(QJsonObject &obj) Q_DECL_OVERRIDE;

    int durationSeconds() const;
    const QMUD::ItemBaseBonus& baseBonus() const Q_DECL_OVERRIDE;

public slots:

private:
    explicit ItemTonic(Entity *parent = Q_NULLPTR);

    int m_iDurationSeconds;
    QMUD::ItemBaseBonus m_itemBaseBonus;
};

} // namespace CORE

#endif // ITEMTONIC_H
