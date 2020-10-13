/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "characterclassmonk.h"
#include "character.h"
#include "../global.h"

using namespace CORE;

CharacterClassMonk::CharacterClassMonk() :
    CharacterClass(QMUD::ClassType::MONK)
{

}

CharacterClassMonk::~CharacterClassMonk()
{

}

QString CharacterClassMonk::toReadableString() const
{
    return "Monaco";
}

int CharacterClassMonk::hitDice() const
{
    return 8;
}

int CharacterClassMonk::manaDice() const
{
    return 0;
}

qint64 CharacterClassMonk::experienceToLevelUp(int level) const
{
    return QMUD::experienceByLevelBase(level);
}

int CharacterClassMonk::maxNumberOfAttacks(int level) const
{
    Q_UNUSED(level)

    return 5;
}

void CharacterClassMonk::AICombatAction(Character *ch,
                                           Character *target) const
{
    Q_UNUSED(ch)
    Q_UNUSED(target)
}

void CharacterClassMonk::AIIdleAction(Character *ch) const
{
    Q_UNUSED(ch)
}

QVector<QMUD::ItemClassType> CharacterClassMonk::itemClasses() const
{
    static QVector<QMUD::ItemClassType> c;

    if (c.isEmpty())
    {
        c.resize(1);
        c[0] = QMUD::ItemClassType::DPS_DEX;
        c.squeeze();
    }

    return c;
}

bool CharacterClassMonk::canUseAbility(int level, QMUD::AbilityType ability) const
{
    Q_UNUSED(level)

    if (ability == QMUD::AbilityType::KICK)
        return true;
    else if (ability == QMUD::AbilityType::DODGE)
        return level >= 5;
    else if (ability == QMUD::AbilityType::PARRY)
        return level >= 25;
    else
        return false;
}

bool CharacterClassMonk::canUseSpell(int level, QMUD::SpellType spell) const
{
    Q_UNUSED(spell)
    Q_UNUSED(level)

    return false;
}

int CharacterClassMonk::hitBonus(int level) const
{
    return level * 2;
}

int CharacterClassMonk::damageBonus(int level) const
{
    return level;
}
