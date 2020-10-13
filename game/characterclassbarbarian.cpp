/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "characterclassbarbarian.h"
#include "character.h"
#include "../global.h"

using namespace CORE;

CharacterClassBarbarian::CharacterClassBarbarian() :
    CharacterClass(QMUD::ClassType::BARBARIAN)
{

}

CharacterClassBarbarian::~CharacterClassBarbarian()
{

}

QString CharacterClassBarbarian::toReadableString() const
{
    return "Barbaro";
}

int CharacterClassBarbarian::hitDice() const
{
    return 12;
}

int CharacterClassBarbarian::manaDice() const
{
    return 0;
}

qint64 CharacterClassBarbarian::experienceToLevelUp(int level) const
{
    return QMUD::experienceByLevelBase(level);
}

int CharacterClassBarbarian::maxNumberOfAttacks(int level) const
{
    Q_UNUSED(level)

    return 4;
}

void CharacterClassBarbarian::AICombatAction(Character *ch, Character *target) const
{
    Q_UNUSED(ch)
    Q_UNUSED(target)
}

void CharacterClassBarbarian::AIIdleAction(Character *ch) const
{
    Q_UNUSED(ch)
}

QVector<QMUD::ItemClassType> CharacterClassBarbarian::itemClasses() const
{
    static QVector<QMUD::ItemClassType> c;

    if (c.isEmpty())
    {
        c.resize(2);
        c[0] = QMUD::ItemClassType::TANK;
        c[1] = QMUD::ItemClassType::DPS_STR;
        c.squeeze();
    }

    return c;
}

bool CharacterClassBarbarian::canUseAbility(int level, QMUD::AbilityType ability) const
{
    if (ability == QMUD::AbilityType::CHARGE)
        return level >= 5;
    else if (ability == QMUD::AbilityType::KICK)
        return level >= 5;
    else
        return false;
}

bool CharacterClassBarbarian::canUseSpell(int level, QMUD::SpellType spell) const
{
    Q_UNUSED(spell)
    Q_UNUSED(level)

    return false;
}

int CharacterClassBarbarian::hitBonus(int level) const
{
    Q_UNUSED(level)

    return level;
}

int CharacterClassBarbarian::damageBonus(int level) const
{
    Q_UNUSED(level)

    return level * 2;
}
