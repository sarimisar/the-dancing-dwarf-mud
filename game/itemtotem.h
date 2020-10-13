/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMTOTEM_H
#define ITEMTOTEM_H

#include "item.h"
#include "../global.h"

#include <QDateTime>

namespace CORE
{

class ItemTotem : public Item
{
    Q_OBJECT
public:
    enum class TotemType
    {
        BEAR,
        EAGLE,
        TIGER,
        ELEMENTAL
    };

    explicit ItemTotem(Entity *parent = Q_NULLPTR);
    virtual ~ItemTotem() Q_DECL_OVERRIDE;

    static ItemTotem* create(int level, TotemType type);

    int level() const;
    TotemType type() const;

public slots:

private:
    int m_iLevel;
    TotemType m_eType;
};

} // namespace CORE

#endif // ITEMTOTEM_H
