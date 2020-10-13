/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMPOTION_H
#define ITEMPOTION_H

#include "item.h"
#include "../global.h"

namespace CORE
{

class ItemPotion : public Item
{
    Q_OBJECT
public:  
    virtual ~ItemPotion() Q_DECL_OVERRIDE;

    static Item* loadItem(const QJsonObject &obj, Entity* parent);
    virtual void writeJson(QJsonObject &obj) Q_DECL_OVERRIDE;

    int durationSeconds() const;

    int hitPoints() const;
    int manaPoints() const;
    int actionPoints() const;

    virtual const QMUD::ItemBaseStatus& baseTemporaryStatus() const;

public slots:

private:
    explicit ItemPotion(Entity *parent = Q_NULLPTR);

    int m_iDurationSeconds;

    int m_iHitPoints;
    int m_iManaPoints;
    int m_iActionPoints;

    QMUD::ItemBaseStatus m_baseTemporaryStatus;
};

} // namespace CORE

#endif // ITEMPOTION_H
