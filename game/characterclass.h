/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#pragma once
#ifndef CHARACTERCLASS_H
#define CHARACTERCLASS_H

#include "../global.h"

#include <QtCore>

namespace CORE
{

class Character;

class CharacterClass
{
public:
    QMUD::ClassType classType() const;

    static CharacterClass* fromString(QString name);
    static CharacterClass* fromType(QMUD::ClassType type);
    static QString toString(QMUD::ClassType type);
    static QString toReadableString(QMUD::ClassType type);

    virtual QString toReadableString() const = 0;
    virtual qint64 experienceToLevelUp(int level) const = 0;

    virtual void AICombatAction(Character* ch, Character* target) const = 0;
    virtual void AIIdleAction(Character* ch) const = 0;
    virtual void AIBuff(Character* ch) const;

    virtual QVector<QMUD::ItemClassType> itemClasses() const = 0;
    virtual int maxNumberOfAttacks(int level) const = 0;
    virtual int hitDice() const = 0;
    virtual int manaDice() const = 0;
    virtual bool canUseAbility(int level, QMUD::AbilityType ability) const = 0;
    virtual bool canUseSpell(int level, QMUD::SpellType spell) const = 0;
    virtual int hitBonus(int level) const;
    virtual int damageBonus(int level) const;
    virtual int handDamageMin(int level) const;
    virtual int handDamageMax(int level) const;

protected:
    CharacterClass(QMUD::ClassType classType);
    virtual ~CharacterClass();

private:
    QMUD::ClassType m_eClassType;
};

}

#endif // CHARACTERCLASS_H
