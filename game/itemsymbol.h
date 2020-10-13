/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef ITEMSYMBOL_H
#define ITEMSYMBOL_H

#include "item.h"
#include "../global.h"

#include <QDateTime>

namespace CORE
{

class ItemSymbol : public Item
{
    Q_OBJECT
public:
    enum class SymbolType
    {
        DIVINE,
        WAR
    };

    explicit ItemSymbol(Entity *parent = Q_NULLPTR);
    virtual ~ItemSymbol() Q_DECL_OVERRIDE;

    int level() const;
    SymbolType type() const;

public slots:

private:
    int m_iLevel;
    SymbolType m_eType;
};

} // namespace CORE

#endif // ITEMSYMBOL_H
