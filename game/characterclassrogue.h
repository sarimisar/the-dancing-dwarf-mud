/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef CHARACTERCLASSROGUE_H
#define CHARACTERCLASSROGUE_H

#include "characterclass.h"

#include <QtCore>

namespace CORE
{


class CharacterClassRogue : public CharacterClass
{
public:
    static CharacterClass* instance() { static CharacterClassRogue c; return &c; }

    virtual QString toReadableString() const Q_DECL_OVERRIDE;
    virtual qint64 experienceToLevelUp(int level) const Q_DECL_OVERRIDE;
    virtual void AICombatAction(Character* ch, Character* target) const Q_DECL_OVERRIDE;
    virtual void AIIdleAction(Character *ch) const Q_DECL_OVERRIDE;

    virtual QVector<QMUD::ItemClassType> itemClasses() const Q_DECL_OVERRIDE;
    virtual int maxNumberOfAttacks(int level) const Q_DECL_OVERRIDE;
    virtual int hitDice() const Q_DECL_OVERRIDE;
    virtual int manaDice() const Q_DECL_OVERRIDE;
    virtual bool canUseAbility(int level, QMUD::AbilityType ability) const Q_DECL_OVERRIDE;
    virtual bool canUseSpell(int level, QMUD::SpellType spell) const Q_DECL_OVERRIDE;

private:
    CharacterClassRogue();
    virtual ~CharacterClassRogue() Q_DECL_OVERRIDE;
};

}

#endif // CHARACTERCLASSROGUE_H
