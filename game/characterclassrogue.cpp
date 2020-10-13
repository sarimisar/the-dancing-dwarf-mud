/*
* QMUD QT Mud
* See COPYRIGHT file at the top of the source tree.
*
* This product includes software developed by the
* Francesco Marcantoni (marcantoni.francesco[at]gmail[dot]com).
*/

#include "characterclassrogue.h"
#include "character.h"
#include "../global.h"

using namespace CORE;

CharacterClassRogue::CharacterClassRogue() :
    CharacterClass(QMUD::ClassType::ROGUE)
{

}

CharacterClassRogue::~CharacterClassRogue()
{

}

QString CharacterClassRogue::toReadableString() const
{
    return "Ladro";
}

int CharacterClassRogue::hitDice() const
{
    return 6;
}

int CharacterClassRogue::manaDice() const
{
    return 0;
}

qint64 CharacterClassRogue::experienceToLevelUp(int level) const
{
    return QMUD::experienceByLevelBase(level);
}

int CharacterClassRogue::maxNumberOfAttacks(int level) const
{
    Q_UNUSED(level)

    return 5;
}

void CharacterClassRogue::AICombatAction(Character *ch, Character *target) const
{
    Q_UNUSED(ch)
    Q_UNUSED(target)
}

void CharacterClassRogue::AIIdleAction(Character *ch) const
{
    Q_UNUSED(ch)
}

QVector<QMUD::ItemClassType> CharacterClassRogue::itemClasses() const
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

bool CharacterClassRogue::canUseAbility(int level, QMUD::AbilityType ability) const
{
    Q_UNUSED(level)
    Q_UNUSED(ability)

    if (ability == QMUD::AbilityType::BACKSTAB)
        return level >= 5;
    else if (ability == QMUD::AbilityType::DODGE)
        return level >= 15;

    return false;
}

bool CharacterClassRogue::canUseSpell(int level, QMUD::SpellType spell) const
{
    Q_UNUSED(spell)
    Q_UNUSED(level)

    return false;
}
